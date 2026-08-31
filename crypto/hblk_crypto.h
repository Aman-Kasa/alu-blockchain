#ifndef HBLK_CRYPTO_H
#define HBLK_CRYPTO_H

#define OPENSSL_SUPPRESS_DEPRECATED

#include <stddef.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>

#ifndef SHA256_DIGEST_LENGTH
#define SHA256_DIGEST_LENGTH 32
#endif

#ifndef EC_CURVE
#define EC_CURVE NID_secp256k1
#endif

uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH]);
EC_KEY *ec_create(void);

#endif /* HBLK_CRYPTO_H */
