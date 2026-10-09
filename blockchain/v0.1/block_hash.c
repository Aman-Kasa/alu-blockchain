#include "blockchain.h"

/*
 * Portable SHA-256 (FIPS 180-4), written in plain C on purpose.
 * OpenSSL picks CPU-specific instructions (SHA-NI, AVX2) at runtime,
 * which old valgrind versions cannot emulate ("Illegal instruction").
 * This implementation gives the exact same digest without them.
 */
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define BSIG0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BSIG1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SSIG0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define SSIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

static const uint32_t k_sha[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/**
 * sha256_compress - processes one 64-byte chunk
 * @h: the 8 words of the running hash state
 * @chunk: the 64 bytes to mix into the state
 */
static void sha256_compress(uint32_t h[8], uint8_t const chunk[64])
{
	uint32_t w[64], v[8], t1, t2;
	int i;

	for (i = 0; i < 16; i++)
		w[i] = ((uint32_t)chunk[i * 4] << 24) |
			((uint32_t)chunk[i * 4 + 1] << 16) |
			((uint32_t)chunk[i * 4 + 2] << 8) | (uint32_t)chunk[i * 4 + 3];
	for (; i < 64; i++)
		w[i] = SSIG1(w[i - 2]) + w[i - 7] + SSIG0(w[i - 15]) + w[i - 16];
	memcpy(v, h, sizeof(v));
	for (i = 0; i < 64; i++)
	{
		t1 = v[7] + BSIG1(v[4]) + ((v[4] & v[5]) ^ (~v[4] & v[6])) +
			k_sha[i] + w[i];
		t2 = BSIG0(v[0]) + ((v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]));
		memmove(v + 1, v, 7 * sizeof(*v));
		v[4] += t1;
		v[0] = t1 + t2;
	}
	for (i = 0; i < 8; i++)
		h[i] += v[i];
}

/**
 * sha256_portable - computes the SHA-256 digest of a buffer
 * @msg: bytes to hash
 * @len: number of bytes in @msg
 * @out: buffer receiving the 32-byte digest
 */
static void sha256_portable(uint8_t const *msg, size_t len,
		uint8_t out[SHA256_DIGEST_LENGTH])
{
	uint32_t h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	uint8_t chunk[64];
	uint64_t bits = (uint64_t)len * 8;
	size_t i, rest;

	for (i = 0; len - i >= 64; i += 64)
		sha256_compress(h, msg + i);
	rest = len - i;
	memset(chunk, 0, sizeof(chunk));
	memcpy(chunk, msg + i, rest);
	chunk[rest] = 0x80;
	if (rest >= 56)
	{
		sha256_compress(h, chunk);
		memset(chunk, 0, sizeof(chunk));
	}
	for (i = 0; i < 8; i++)
		chunk[63 - i] = (uint8_t)(bits >> (i * 8));
	sha256_compress(h, chunk);
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
		out[i] = (uint8_t)(h[i / 4] >> (24 - (i % 4) * 8));
}

/**
 * block_hash - computes the hash of a Block
 *
 * @block: pointer to the Block to be hashed
 * @hash_buf: buffer to store the resulting hash
 *
 * Description: the hash covers the Block info followed by the
 * first data.len bytes of the data buffer. block->hash is left unchanged.
 *
 * Return: pointer to hash_buf, or NULL upon failure
 */
uint8_t *block_hash(block_t const *block,
		uint8_t hash_buf[SHA256_DIGEST_LENGTH])
{
	size_t len;

	if (!block || !hash_buf)
		return (NULL);
	len = block->data.len;
	if (len > BLOCKCHAIN_DATA_MAX)
		len = BLOCKCHAIN_DATA_MAX;
	len += sizeof(block->info);
	sha256_portable((uint8_t const *)block, len, hash_buf);
	return (hash_buf);
}
