#include "blockchain.h"

#define GENESIS_DATA "Holberton School"
#define GENESIS_DATA_LEN 16
#define GENESIS_HASH "\xc5\x2c\x26\xc8\xb5\x46\x16\x39\x63\x5d\x8e\xdf" \
	"\x2a\x97\xd4\x8d\x0c\x8e\x00\x09\xc8\x17\xf2\xb1\xd3\xd7\xff\x2f" \
	"\x04\x51\x58\x03"
#define GENESIS_TIMESTAMP 1537578000

/**
 * is_genesis - checks whether a Block matches the Genesis Block
 * @block: pointer to the Block to check
 *
 * Description: compares field by field so that struct padding
 * never affects the result
 *
 * Return: 1 if it matches, 0 otherwise
 */
static int is_genesis(block_t const *block)
{
	uint8_t zero[SHA256_DIGEST_LENGTH] = {0};

	return (block->info.index == 0 && block->info.difficulty == 0 &&
		block->info.timestamp == GENESIS_TIMESTAMP &&
		block->info.nonce == 0 &&
		!memcmp(block->info.prev_hash, zero, SHA256_DIGEST_LENGTH) &&
		block->data.len == GENESIS_DATA_LEN &&
		!memcmp(block->data.buffer, GENESIS_DATA, GENESIS_DATA_LEN) &&
		!memcmp(block->hash, GENESIS_HASH, SHA256_DIGEST_LENGTH));
}

/**
 * block_is_valid - verifies that a Block is valid
 * @block: pointer to the Block to check
 * @prev_block: pointer to the previous Block, or NULL if @block is first
 *
 * Description: v0.2 also checks the hash against the Block difficulty
 *
 * Return: 0 if the Block is valid, 1 otherwise
 */
int block_is_valid(block_t const *block, block_t const *prev_block)
{
	uint8_t hash_buf[SHA256_DIGEST_LENGTH];

	if (!block)
		return (1);
	if (block->info.index == 0)
		return (prev_block || !is_genesis(block));
	if (!prev_block)
		return (1);
	if (block->data.len > BLOCKCHAIN_DATA_MAX ||
		prev_block->data.len > BLOCKCHAIN_DATA_MAX)
		return (1);
	if (block->info.index != prev_block->info.index + 1)
		return (1);
	if (!block_hash(prev_block, hash_buf) ||
		memcmp(hash_buf, prev_block->hash, SHA256_DIGEST_LENGTH))
		return (1);
	if (memcmp(hash_buf, block->info.prev_hash, SHA256_DIGEST_LENGTH))
		return (1);
	if (!block_hash(block, hash_buf) ||
		memcmp(hash_buf, block->hash, SHA256_DIGEST_LENGTH) ||
		!hash_matches_difficulty(block->hash, block->info.difficulty))
		return (1);
	return (0);
}
