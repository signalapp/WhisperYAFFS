/*
 * Copyright 2004 Clemens Fruhwirth <clemens@endorphin.org>
 * Copyright 2006 Ivan Deras Tabora  <ideras@gmail.com> (adapted to work with Linux Crypto API)
 * Copyright 2011 Whisper Systems   <moxie@whispersys.com> (minor adaptions for YAFFS2)
 *
 * Implementation of PBKDF2-HMAC-SHA1 according to RFC 2898.
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

#include "linux/crypto.h"
#include "linux/types.h"
#include "linux/scatterlist.h"

#include "yaffs_pbkdf2.h"
#include "yaffs_crypto.h"

static void inline xor_block(u8 const *src1, u8 const *src2, char *dst, size_t n)
{
	while (n--)
		*dst++ = *src1++ ^ *src2++;
}

/*
 * This function implements PBKDF2-HMAC-SHA1 according to RFC 2898
 */
int yaffs_pbkdf2(const char *pwd, size_t pwd_len,
		 const char *salt, size_t salt_len,
		 unsigned int iterations,
		 char *d_key, size_t d_key_len)
{
	/* U_n is the buffer for U_n values */
	unsigned char U_n[SHA1_DIGEST_SIZE];
	/* F_buf is the XOR buffer for F function */
	char F_buf[SHA1_DIGEST_SIZE];
	struct scatterlist sg[4];
	u32 j, i = 1, i_network_ordered;
	struct hash_desc desc;
	int retval;

	struct crypto_hash *hash_tfm;

	hash_tfm   = crypto_alloc_hash("hmac(sha1)",0,CRYPTO_ALG_ASYNC);
	desc.tfm   = hash_tfm;
	desc.flags = 0;

	/* The first hash iteration is done different, therefore
	   we reduce iterations to conveniently use it as a loop
	   counter */
	iterations--;

	/* Setup the scatterlists */
	sg_set_buf(&sg[0], (void*)pwd, pwd_len);
	sg_set_buf(&sg[1], (void*)salt, salt_len);
	sg_set_buf(&sg[2], (void*)&i_network_ordered, sizeof(u32));
	sg_set_buf(&sg[3], (void*)U_n, SHA1_DIGEST_SIZE);

	crypto_hash_init(&desc);
	crypto_hash_setkey(desc.tfm, (u8 *)pwd, pwd_len);
	crypto_hash_final(&desc, U_n);
	memset(U_n, 0, SHA1_DIGEST_SIZE);

	while(d_key_len > 0) {
		unsigned int blocksize = d_key_len<SHA1_DIGEST_SIZE? d_key_len : SHA1_DIGEST_SIZE;

		j = iterations;

		// U_1 hashing
		i_network_ordered = cpu_to_be32(i);

		crypto_hash_init(&desc);
		crypto_hash_setkey(desc.tfm, (u8 *)pwd, pwd_len);
		crypto_hash_update(&desc, &sg[1], salt_len);

		crypto_hash_update(&desc, &sg[2], sizeof(u32));
		crypto_hash_final(&desc, U_n);

		memcpy(F_buf, U_n, SHA1_DIGEST_SIZE);

		// U_n hashing
		while(j--) {
			crypto_hash_init(&desc);
			crypto_hash_setkey(hash_tfm, (u8 *)pwd, pwd_len);
			crypto_hash_digest(&desc, &sg[3], SHA1_DIGEST_SIZE, U_n);

			xor_block((u8 *)F_buf, (u8 *)U_n, F_buf, SHA1_DIGEST_SIZE);
		}
		memcpy(d_key, F_buf, blocksize);

		d_key += blocksize;
		d_key_len -= blocksize;
		i++;
	}

	crypto_free_hash(hash_tfm);

	retval = 0;
	return retval;
}
