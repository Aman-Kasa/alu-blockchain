#include "blockchain.h"

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
	return (sha256((int8_t const *)block, len, hash_buf));
}
