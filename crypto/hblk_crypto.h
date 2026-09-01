#ifndef _HBLK_CRYPTO_H_
#define _HBLK_CRYPTO_H_

#include <stddef.h>
#include <stdint.h>
#include <openssl/sha.h>

uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH]);

#endif /* _HBLK_CRYPTO_H_ */
