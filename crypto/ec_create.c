#include "hblk_crypto.h"

/**
 * ec_create - Creates a new EC key pair using the secp256k1 curve
 *
 * Return: Pointer to the created EC_KEY structure, or NULL upon failure
 */
EC_KEY *ec_create(void)
{
	EC_KEY *key = NULL;

	key = EC_KEY_new_by_curve_name(EC_CURVE);
	if (!key)
		return (NULL);

	if (!EC_KEY_generate_key(key))
	{
		EC_KEY_free(key);
		return (NULL);
	}

	return (key);
}
