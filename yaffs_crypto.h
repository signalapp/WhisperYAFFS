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

#include "linux/crypto.h"

struct yaffs_key_descriptor_block {
	u32 version_number;
	u32 iteration_count;

	u8 key_salt[16];
	u8 mac_salt[16];

	u8 iv[AES_BLOCK_SIZE];
	u8 keys[XTS_KEY_SIZE];

	u8 mac[SHA1_DIGEST_SIZE];
};


void AES_xts_encrypt(struct crypto_blkcipher *cipher,
		     const u8 *page_plaintext, u8 *page_ciphertext,
		     int page_tweak, int page_size,
		     const u8 *tags_plaintext, u8 *tags_ciphertext,
		     int tags_tweak, int tags_size);

void AES_xts_decrypt(struct crypto_blkcipher *cipher,
		     u8 *page_ciphertext, u8 *page_plaintext,
		     int page_tweak, int page_size,
		     u8 *tags_ciphertext, u8 *tags_plaintext,
		     int tags_tweak, int tags_size);

int yaffs_generate_keys(u8 *key_buffer, int key_size);

int yaffs_encrypt_keys_to_page(char *password,
			       u8 *page, int page_length,
			       u8 *keys, int keys_length);

int yaffs_decrypt_keys_from_page(char *password, u8 *page, u8 *keys);



#endif
