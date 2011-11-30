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

static void initialize_tweak_bytes(u8 *tweak_bytes, int tweak)
{
	int j;

	for (j=0;j<AES_BLOCK_SIZE;j++) {
		tweak_bytes[j] = (u8) (tweak & 0xFF);
		tweak = tweak >> 8;
	}
}

static void HMAC_sha1(const u8 *key, int key_length,
		      void *input, int input_length,
		      u8 *output)
{
	struct scatterlist sg[1];
	struct hash_desc desc;
	struct crypto_hash *hash_tfm = crypto_alloc_hash("hmac(sha1)", 0, CRYPTO_ALG_ASYNC);

	desc.tfm = hash_tfm;
	desc.flags = 0;

	sg_set_buf(&sg[0], input, input_length);

	crypto_hash_init(&desc);
	crypto_hash_setkey(desc.tfm, key, key_length);
	crypto_hash_digest(&desc, &sg[0], input_length, output);

	crypto_free_hash(hash_tfm);
}

static void AES_cbc(const u8 *iv, int iv_length,
		    const u8 *key, int key_length,
		    const u8 *input, int input_length,
		    u8 *output, int encrypt)
{
	struct scatterlist src[1];
	struct scatterlist dst[1];
	struct blkcipher_desc desc;
	struct crypto_blkcipher *cipher = crypto_alloc_blkcipher("cbc(aes)", 0, 0);

	crypto_blkcipher_setkey(cipher, key, key_length);

	sg_init_table(dst, 1);
	sg_init_table(src, 1);

	sg_set_buf(&dst[0], output, input_length);
	sg_set_buf(&src[0], input, input_length);

	desc.tfm = cipher;
	desc.flags = 0;

	crypto_blkcipher_set_iv(cipher, iv, iv_length);

	if (encrypt)
		crypto_blkcipher_encrypt(&desc, dst, src, input_length);
	else
		crypto_blkcipher_decrypt(&desc, dst, src, input_length);

	crypto_free_blkcipher(cipher);
}

static void AES_xts(struct crypto_blkcipher *cipher, int tweak,
		    const u8 *input, u8 *output,
		    int length, int encrypt)
{
	struct blkcipher_desc desc;
	struct scatterlist dst[1];
	struct scatterlist src[1];

	u8 tweak_bytes[AES_BLOCK_SIZE];
	initialize_tweak_bytes(tweak_bytes, tweak);

	sg_init_table(dst, 1);
	sg_init_table(src, 1);

	sg_set_buf(&dst[0], output, length);
	sg_set_buf(&src[0], input, length);

	desc.tfm = cipher;
	desc.flags = 0;
	desc.info = tweak_bytes;

	if (encrypt)
		crypto_blkcipher_encrypt_iv(&desc, &dst[0], &src[0], length);
	else
		crypto_blkcipher_decrypt_iv(&desc, &dst[0], &src[0], length);
}

void AES_xts_encrypt(struct crypto_blkcipher *cipher,
		     const u8 *page_plaintext, u8 *page_ciphertext,
		     int page_tweak, int page_size,
		     const u8 *tags_plaintext, u8 *tags_ciphertext,
		     int tags_tweak, int tags_size)
{
	int missing_bytes;
	u8 block_aligned_tags_plaintext[AES_BLOCK_SIZE];
	u8 block_aligned_tags_ciphertext[AES_BLOCK_SIZE];

	AES_xts(cipher, page_tweak,
		page_plaintext, page_ciphertext,
		page_size, 1);

	missing_bytes = sizeof(block_aligned_tags_plaintext) - tags_size;

	memcpy(block_aligned_tags_plaintext, tags_plaintext, tags_size);
	memcpy(block_aligned_tags_plaintext + tags_size,
	       page_ciphertext + (page_size - missing_bytes),
	       missing_bytes);

	AES_xts(cipher, tags_tweak,
		block_aligned_tags_plaintext, block_aligned_tags_ciphertext,
		sizeof(block_aligned_tags_plaintext), 1);

	memcpy(page_ciphertext + (page_size-missing_bytes),
	       block_aligned_tags_ciphertext,
	       missing_bytes);
	memcpy(tags_ciphertext, block_aligned_tags_ciphertext + missing_bytes,
	       tags_size);
}

void AES_xts_decrypt(struct crypto_blkcipher *cipher,
		     u8 *page_ciphertext, u8 *page_plaintext,
		     int page_tweak, int page_size,
		     u8 *tags_ciphertext, u8 *tags_plaintext,
		     int tags_tweak, int tags_size)
{
	int missing_bytes;
	u8 block_aligned_tags_ciphertext[AES_BLOCK_SIZE];
	u8 block_aligned_tags_plaintext[AES_BLOCK_SIZE];

	missing_bytes = sizeof(block_aligned_tags_ciphertext) - tags_size;

	memcpy(block_aligned_tags_ciphertext,
	       page_ciphertext + (page_size - missing_bytes),
	       missing_bytes);
	memcpy(block_aligned_tags_ciphertext + missing_bytes,
	       tags_ciphertext,
	       tags_size);

	AES_xts(cipher, tags_tweak,
		block_aligned_tags_ciphertext, block_aligned_tags_plaintext,
		sizeof(block_aligned_tags_ciphertext), 0);

	memcpy(tags_plaintext, block_aligned_tags_plaintext, tags_size);
	memcpy(page_ciphertext + (page_size - missing_bytes),
	       block_aligned_tags_plaintext + tags_size,
	       missing_bytes);

	AES_xts(cipher, page_tweak,
		page_ciphertext, page_plaintext,
		page_size, 0);
}

int yaffs_generate_keys(u8 *key_buffer, int key_size)
{
	get_random_bytes(key_buffer, key_size);
	return 1;
}

int yaffs_decrypt_keys_from_page(char *password, u8 *page, u8 *keys)
{
	int i;
	u8 password_key[16];
	u8 mac_key[20];
	u8 our_mac[20];

	struct yaffs_key_descriptor_block *key_block;
	key_block = (struct yaffs_key_descriptor_block *)page;

	if (key_block->version_number != 1) {
		printk(KERN_INFO "Version number (%d) on block is not 1!", key_block->version_number);

		printk(KERN_INFO "Descriptor dump:\n");
		for (i=0;i<sizeof(struct yaffs_key_descriptor_block);i++)
			printk("0x%02x, ", page[i]);

		printk(KERN_INFO "\n");

		return -1;
	}

	printk(KERN_INFO "Pulled key salt:\n");
	for (i=0;i<sizeof(key_block->key_salt);i++)
		printk("0x%02x, ", key_block->key_salt[i]);

	printk(KERN_INFO "\n");

	printk(KERN_INFO "Pulled mac salt:\n");
	for (i=0;i<sizeof(key_block->mac_salt);i++)
		printk("0x%02x, ", key_block->mac_salt[i]);

	printk(KERN_INFO "\n");

	printk(KERN_INFO "Pulled their MAC:\n");
	for (i=0;i<sizeof(key_block->mac);i++)
		printk("0x%02x, ", key_block->mac[i]);

	printk(KERN_INFO "\n");

	yaffs_pbkdf2(password, strlen(password),
		     key_block->key_salt, sizeof(key_block->key_salt),
		     key_block->iteration_count,
		     password_key, sizeof(password_key));

	yaffs_pbkdf2(password, strlen(password),
		     key_block->mac_salt, sizeof(key_block->mac_salt),
		     key_block->iteration_count,
		     mac_key, sizeof(mac_key));

	HMAC_sha1(mac_key, sizeof(mac_key),
		  key_block,
		  sizeof(struct yaffs_key_descriptor_block) - sizeof(key_block->mac),
		  our_mac);

	if (memcmp(our_mac, key_block->mac, sizeof(our_mac)) != 0) {
		printk(KERN_INFO "Calculated mac does not match!\nOur mac:\n");
		for (i=0;i<sizeof(our_mac);i++)
			printk("0x%02x, ", our_mac[i]);

		printk(KERN_INFO "\n");

		printk(KERN_INFO "Their mac:\n");
		for (i=0;i<sizeof(key_block->mac);i++)
			printk("0x%02x, ", key_block->mac[i]);

		printk(KERN_INFO "\n");

		return -1;
	}

	AES_cbc(password_key, sizeof(password_key),
		key_block->iv, sizeof(key_block->iv),
		key_block->keys, sizeof(key_block->keys),
		keys, 0);

	return 1;
}

int yaffs_encrypt_keys_to_page(char *password,
			    u8 *page, int page_length,
			    u8 *keys, int keys_length)
{
	int i;
	u8 password_key[16];
	u8 mac_key[20];

	struct yaffs_key_descriptor_block *key_block;

	if (sizeof(struct yaffs_key_descriptor_block) > page_length)
		return -1;

	key_block = (struct yaffs_key_descriptor_block *)page;
	key_block->version_number = 1;
	key_block->iteration_count = PASSWORD_ITERATIONS;

	get_random_bytes(key_block->key_salt, sizeof(key_block->key_salt));
	get_random_bytes(key_block->mac_salt, sizeof(key_block->mac_salt));
	get_random_bytes(key_block->iv, sizeof(key_block->iv));

	printk(KERN_INFO "Generated key salt:\n");
	for (i=0;i<sizeof(key_block->key_salt);i++)
		printk("0x%02x, ", key_block->key_salt[i]);

	printk(KERN_INFO "\n");

	printk(KERN_INFO "Generated mac salt:\n");
	for (i=0;i<sizeof(key_block->mac_salt);i++)
		printk("0x%02x, ", key_block->mac_salt[i]);

	printk(KERN_INFO "\n");

	printk(KERN_INFO "Generated IV:\n");
	for (i=0;i<sizeof(key_block->iv);i++)
		printk("0x%02x, ", key_block->iv[i]);

	printk(KERN_INFO "\n");

	yaffs_pbkdf2(password, strlen(password),
		     key_block->key_salt, sizeof(key_block->key_salt),
		     PASSWORD_ITERATIONS, password_key, sizeof(password_key));

	yaffs_pbkdf2(password, strlen(password),
		     key_block->mac_salt, sizeof(key_block->mac_salt),
		     PASSWORD_ITERATIONS, mac_key, sizeof(mac_key));

	AES_cbc(password_key, sizeof(password_key),
		key_block->iv, sizeof(key_block->iv),
		keys, keys_length,
		key_block->keys, 1);

	HMAC_sha1(mac_key, sizeof(mac_key),
		  key_block,
		  sizeof(struct yaffs_key_descriptor_block) - sizeof(key_block->mac),
		  key_block->mac);

	printk(KERN_INFO "Generated MAC:\n");
	for (i=0;i<sizeof(key_block->mac);i++)
		printk("0x%02x, ", key_block->mac[i]);

	printk(KERN_INFO "\n");

	return 1;
}
