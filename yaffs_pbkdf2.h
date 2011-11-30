int yaffs_pbkdf2(const char *pwd, size_t pwd_len,
		 const char *salt, size_t salt_len,
		 unsigned int iterations,
		 char *d_key, size_t d_key_len);
