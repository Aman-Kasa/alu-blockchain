#include "hblk_crypto.h"

/**
 * ec_to_pub - Extracts the public key from an EC_KEY structure
 * @key: Pointer to the EC_KEY structure to retrieve the public key from
 * @pub: Address at which to store the extracted public key
 *
 * Return: Pointer to pub, or NULL upon failure
 */
uint8_t *ec_to_pub(EC_KEY const *key, uint8_t pub[EC_PUB_LEN])
{
	const EC_POINT *point;
	const EC_GROUP *group;

	if (!key || !pub)
		return (NULL);

	point = EC_KEY_get0_public_key(key);
	group = EC_KEY_get0_group(key);
	if (!point || !group)
		return (NULL);

	if (EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED,
			       pub, EC_PUB_LEN, NULL) != EC_PUB_LEN)
		return (NULL);

	return (pub);
}
