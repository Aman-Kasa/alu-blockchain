#include "blockchain.h"

/**
 * hash_matches_difficulty - checks whether a hash matches a difficulty
 * @hash: hash to check
 * @difficulty: minimum number of leading zero bits the hash must have
 *
 * Description: the first difficulty / 8 bytes must be zero, then the
 * top difficulty % 8 bits of the next byte must be zero. When the
 * difficulty is a multiple of 8 there is no partial byte to check.
 *
 * Return: 1 if the difficulty is respected, 0 otherwise
 */
int hash_matches_difficulty(uint8_t const hash[SHA256_DIGEST_LENGTH],
	uint32_t difficulty)
{
	uint32_t i, full_bytes, extra_bits;

	if (!hash || difficulty > SHA256_DIGEST_LENGTH * 8)
		return (0);
	full_bytes = difficulty / 8;
	extra_bits = difficulty % 8;
	for (i = 0; i < full_bytes; i++)
	{
		if (hash[i] != 0)
			return (0);
	}
	if (extra_bits && (hash[full_bytes] >> (8 - extra_bits)) != 0)
		return (0);
	return (1);
}
