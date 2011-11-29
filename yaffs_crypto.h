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

#ifndef __YAFFS_CRYPTO_H__
#define __YAFFS_CRYPTO_H__

#define AES_BLOCK_SIZE 16
#define XTS_KEY_SIZE 32
#define SHA1_DIGEST_SIZE 20

#include "devextras.h"
#include "linux/crypto.h"

typedef struct {
  __u32 versionNumber;
  __u32 iterationCount;

  __u8 keySalt[16];     
  __u8 macSalt[16];

  __u8 iv[AES_BLOCK_SIZE];
  __u8 keys[XTS_KEY_SIZE];

  __u8 mac[SHA1_DIGEST_SIZE];
} yaffs_KeyDescriptorBlock;


void AES_xts_encrypt(struct crypto_blkcipher *cipher, 
		     const __u8 *pagePlaintext, __u8 *pageCiphertext, int pageTweak, int pageSize,
		     const __u8 *tagsPlaintext, __u8 *tagsCiphertext, int tagsTweak, int tagsSize);

void AES_xts_decrypt(struct crypto_blkcipher *cipher, 
		     __u8 *pageCiphertext, __u8 *pagePlaintext, int pageTweak, int pageSize,
		     __u8 *tagsCiphertext, __u8 *tagsPlaintext, int tagsTweak, int tagsSize); 

int yaffs_GenerateKeys(__u8 *keyBuffer, int keySize);

int yaffs_EncryptKeysToPage(char *password,
			    __u8 *page, int pageLength,
			    __u8 *keys, int keysLength);

int yaffs_DecryptKeysFromPage(char *password, __u8 *page, __u8 *keys);



#endif
