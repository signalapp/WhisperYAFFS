/*
 * Copyright (C) 2011 Whisper Systems <moxie@whispersys.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * This code has two responsibilities:
 *
 * 1) Password-based encrypting / decrypting the key descriptor block for
 *    the FS.
 *
 * 2) Encrypting / Decrypting FS pages as they are written to / from disk.
 *
 */

#include "linux/scatterlist.h"
#include "linux/random.h"
#include "linux/string.h"

#include "yaffs_crypto.h"
#include "yaffs_pbkdf2.h"

#define PASSWORD_ITERATIONS 4000

static void initializeTweakBytes(__u8 *tweakBytes, int tweak) {
  int j;

  for (j=0;j<AES_BLOCK_SIZE;j++) {
    tweakBytes[j] = (__u8) (tweak & 0xFF); 
    tweak         = tweak >> 8;
  }
}

static void HMAC_sha1(const __u8 *key, int keyLength,
		      void *input, int inputLength,
		      __u8 *output)
{
  struct scatterlist sg[1];
  struct hash_desc desc;
  struct crypto_hash *hash_tfm = crypto_alloc_hash("hmac(sha1)", 0, CRYPTO_ALG_ASYNC);

  desc.tfm   = hash_tfm;
  desc.flags = 0;
  
  sg_set_buf(&sg[0], input, inputLength);

  crypto_hash_init(&desc);
  crypto_hash_setkey(desc.tfm, key, keyLength);
  crypto_hash_digest(&desc, &sg[0], inputLength, output);

  crypto_free_hash(hash_tfm);
}

static void AES_cbc(const __u8 *iv, int ivLength, 
		    const __u8 *key, int keyLength,
		    const __u8 *input, int inputLength, 
		    __u8 *output, int encrypt)
{
  struct scatterlist src[1];
  struct scatterlist dst[1];
  struct blkcipher_desc desc;
  struct crypto_blkcipher *cipher = crypto_alloc_blkcipher("cbc(aes)", 0, 0);

  crypto_blkcipher_setkey(cipher, key, keyLength);

  sg_init_table(dst, 1);
  sg_init_table(src, 1);

  sg_set_buf(&dst[0], output, inputLength);
  sg_set_buf(&src[0], input, inputLength);

  desc.tfm   = cipher;
  desc.flags = 0;
  
  crypto_blkcipher_set_iv(cipher, iv, ivLength);
  
  if (encrypt)
    crypto_blkcipher_encrypt(&desc, dst, src, inputLength);
  else
    crypto_blkcipher_decrypt(&desc, dst, src, inputLength);

  crypto_free_blkcipher(cipher);
}

static void AES_xts(struct crypto_blkcipher *cipher, int tweak, const __u8 *input, __u8 *output, int length, int encrypt) {
  struct blkcipher_desc desc;
  struct scatterlist dst[1];
  struct scatterlist src[1];

  __u8 tweakBytes[AES_BLOCK_SIZE];
  initializeTweakBytes(tweakBytes, tweak);

  sg_init_table(dst, 1);
  sg_init_table(src, 1);

  sg_set_buf(&dst[0], output, length);
  sg_set_buf(&src[0], input, length);

  desc.tfm   = cipher;
  desc.flags = 0;
  desc.info  = tweakBytes;
  
  if (encrypt)
    crypto_blkcipher_encrypt_iv(&desc, &dst[0], &src[0], length);
  else
    crypto_blkcipher_decrypt_iv(&desc, &dst[0], &src[0], length);
}

void AES_xts_encrypt(struct crypto_blkcipher *cipher, 
		     const __u8 *pagePlaintext, __u8 *pageCiphertext, int pageTweak, int pageSize,
		     const __u8 *tagsPlaintext, __u8 *tagsCiphertext, int tagsTweak, int tagsSize) 
{
  int missingBytes;
  __u8 blockAlignedTagsPlaintext[AES_BLOCK_SIZE];
  __u8 blockAlignedTagsCiphertext[AES_BLOCK_SIZE];

  AES_xts(cipher, pageTweak, pagePlaintext, pageCiphertext, pageSize, 1);
  
  missingBytes = sizeof(blockAlignedTagsPlaintext) - tagsSize;

  memcpy(blockAlignedTagsPlaintext, tagsPlaintext, tagsSize);
  memcpy(blockAlignedTagsPlaintext+tagsSize, pageCiphertext+(pageSize-missingBytes), missingBytes);

  AES_xts(cipher, tagsTweak, blockAlignedTagsPlaintext, blockAlignedTagsCiphertext, sizeof(blockAlignedTagsPlaintext), 1);

  memcpy(pageCiphertext+(pageSize-missingBytes), blockAlignedTagsCiphertext, missingBytes);
  memcpy(tagsCiphertext, blockAlignedTagsCiphertext+missingBytes, tagsSize);
}

void AES_xts_decrypt(struct crypto_blkcipher *cipher, 
		     __u8 *pageCiphertext, __u8 *pagePlaintext, int pageTweak, int pageSize,
		     __u8 *tagsCiphertext, __u8 *tagsPlaintext, int tagsTweak, int tagsSize) 
{
  int missingBytes;
  __u8 blockAlignedTagsCiphertext[AES_BLOCK_SIZE];
  __u8 blockAlignedTagsPlaintext[AES_BLOCK_SIZE];

  missingBytes = sizeof(blockAlignedTagsCiphertext) - tagsSize;
  
  memcpy(blockAlignedTagsCiphertext, pageCiphertext+(pageSize-missingBytes), missingBytes);
  memcpy(blockAlignedTagsCiphertext+missingBytes, tagsCiphertext, tagsSize);

  AES_xts(cipher, tagsTweak, blockAlignedTagsCiphertext, blockAlignedTagsPlaintext, sizeof(blockAlignedTagsCiphertext), 0);

  memcpy(tagsPlaintext, blockAlignedTagsPlaintext, tagsSize);
  memcpy(pageCiphertext+(pageSize-missingBytes), blockAlignedTagsPlaintext+tagsSize, missingBytes);

  AES_xts(cipher, pageTweak, pageCiphertext, pagePlaintext, pageSize, 0);  
}

int yaffs_GenerateKeys(__u8 *keyBuffer, int keySize) {
  get_random_bytes(keyBuffer, keySize);
  return 1;
}

int yaffs_DecryptKeysFromPage(char *password, __u8 *page, __u8 *keys) 
{
  int i;
  __u8 passwordKey[16];
  __u8 macKey[20];
  __u8 ourMac[20];
  
  yaffs_KeyDescriptorBlock *keyBlock = (yaffs_KeyDescriptorBlock*)page;

  if (keyBlock->versionNumber != 1) {
    printk(KERN_INFO "Version number (%d) on block is not 1!", keyBlock->versionNumber);

    printk(KERN_INFO "Descriptor dump:\n");
    for (i=0;i<sizeof(yaffs_KeyDescriptorBlock);i++)
      printk("0x%02x, ", page[i]);
    
    printk(KERN_INFO "\n");

    return -1;
  }

  printk(KERN_INFO "Pulled key salt:\n");
  for (i=0;i<sizeof(keyBlock->keySalt);i++)
    printk("0x%02x, ", keyBlock->keySalt[i]);

  printk(KERN_INFO "\n");

  printk(KERN_INFO "Pulled mac salt:\n");
  for (i=0;i<sizeof(keyBlock->macSalt);i++)
    printk("0x%02x, ", keyBlock->macSalt[i]);

  printk(KERN_INFO "\n");

  printk(KERN_INFO "Pulled their MAC:\n");
  for (i=0;i<sizeof(keyBlock->mac);i++)
    printk("0x%02x, ", keyBlock->mac[i]);

  printk(KERN_INFO "\n");

  yaffs_pbkdf2(password, strlen(password), 
	       keyBlock->keySalt, sizeof(keyBlock->keySalt),
  	       keyBlock->iterationCount, passwordKey, sizeof(passwordKey));

  yaffs_pbkdf2(password, strlen(password), 
	       keyBlock->macSalt, sizeof(keyBlock->macSalt),
  	       keyBlock->iterationCount, macKey, sizeof(macKey));

  HMAC_sha1(macKey, sizeof(macKey),
	    keyBlock, sizeof(yaffs_KeyDescriptorBlock) - sizeof(keyBlock->mac),
	    ourMac);

  if (memcmp(ourMac, keyBlock->mac, sizeof(ourMac)) != 0) {
    printk(KERN_INFO "Calculated mac does not match!\nOur mac:\n");
    for (i=0;i<sizeof(ourMac);i++)
      printk("0x%02x, ", ourMac[i]);
    
    printk(KERN_INFO "\n");

    printk(KERN_INFO "Their mac:\n");
    for (i=0;i<sizeof(keyBlock->mac);i++)
      printk("0x%02x, ", keyBlock->mac[i]);
    
    printk(KERN_INFO "\n");


    return -1;
  }
  
  AES_cbc(passwordKey, sizeof(passwordKey),
	  keyBlock->iv, sizeof(keyBlock->iv),
	  keyBlock->keys, sizeof(keyBlock->keys),
	  keys, 0);
  
  return 1;
}

int yaffs_EncryptKeysToPage(char *password,
			    __u8 *page, int pageLength,
			    __u8 *keys, int keysLength)
{
  int i;
  __u8 passwordKey[16];
  __u8 macKey[20];

  yaffs_KeyDescriptorBlock *keyBlock;

  if (sizeof(yaffs_KeyDescriptorBlock) > pageLength)
    return -1;

  keyBlock                 = (yaffs_KeyDescriptorBlock*)page;  
  keyBlock->versionNumber  = 1;
  keyBlock->iterationCount = PASSWORD_ITERATIONS;

  get_random_bytes(keyBlock->keySalt, sizeof(keyBlock->keySalt));
  get_random_bytes(keyBlock->macSalt, sizeof(keyBlock->macSalt));
  get_random_bytes(keyBlock->iv, sizeof(keyBlock->iv));

  printk(KERN_INFO "Generated key salt:\n");
  for (i=0;i<sizeof(keyBlock->keySalt);i++)
    printk("0x%02x, ", keyBlock->keySalt[i]);

  printk(KERN_INFO "\n");

  printk(KERN_INFO "Generated mac salt:\n");
  for (i=0;i<sizeof(keyBlock->macSalt);i++)
    printk("0x%02x, ", keyBlock->macSalt[i]);

  printk(KERN_INFO "\n");

  printk(KERN_INFO "Generated IV:\n");
  for (i=0;i<sizeof(keyBlock->iv);i++)
    printk("0x%02x, ", keyBlock->iv[i]);

  printk(KERN_INFO "\n");

  yaffs_pbkdf2(password, strlen(password), 
	       keyBlock->keySalt, sizeof(keyBlock->keySalt),
  	       PASSWORD_ITERATIONS, passwordKey, sizeof(passwordKey));

  yaffs_pbkdf2(password, strlen(password), 
	       keyBlock->macSalt, sizeof(keyBlock->macSalt),
  	       PASSWORD_ITERATIONS, macKey, sizeof(macKey));
    
  AES_cbc(passwordKey, sizeof(passwordKey),
	  keyBlock->iv, sizeof(keyBlock->iv), 
	  keys, keysLength, 
	  keyBlock->keys, 1);
  
  HMAC_sha1(macKey, sizeof(macKey),
	    keyBlock, sizeof(yaffs_KeyDescriptorBlock) - sizeof(keyBlock->mac),
	    keyBlock->mac);

  printk(KERN_INFO "Generated MAC:\n");
  for (i=0;i<sizeof(keyBlock->mac);i++)
    printk("0x%02x, ", keyBlock->mac[i]);

  printk(KERN_INFO "\n");

  return 1;
}
