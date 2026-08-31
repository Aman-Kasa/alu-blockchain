#include "hblk_crypto.h"

/**
 * sha256 - Computes the SHA-256 hash of a sequence of bytes
 * @s: sequence of bytes to be hashed
 * @len: number of bytes to hash in s
 * @digest: resulting hash storage
 *
 * Return: pointer to digest, or NULL if digest is NULL
 */
uint8_t *sha256(int8_t const *s, size_t len,
		uint8_t digest[SHA256_DIGEST_LENGTH])
{
	if (!digest)
		return (NULL);

	if (!SHA256((unsigned char const *)s, len, digest))
		return (NULL);

	return (digest);
}
