#include "hblk_crypto.h"

/**
 * sha256 - Computes the SHA-256 hash of a sequence of bytes
 * @s: Pointer to sequence of bytes to hash
 * @len: Number of bytes to hash
 * @digest: Buffer where the resulting hash is stored
 *
 * Return: Pointer to digest, or NULL if digest is NULL
 */

uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH])
{
	if (!digest)
		return (NULL);

	SHA256((unsigned char const *)s, len, digest);
	return (digest);
}
