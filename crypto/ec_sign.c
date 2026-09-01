#include <string.h>
#include "hblk_crypto.h"

/**
 * ec_sign - Signs a given set of bytes using an EC_KEY private key
 * @key: Pointer to the EC_KEY structure containing the private key
 * @msg: Pointer to the msglen characters to be signed
 * @msglen: Number of bytes to sign
 * @sig: Address at which to store the signature
 *
 * Return: Pointer to the signature buffer (sig->sig) upon success,
 *         or NULL upon failure
 */
uint8_t *ec_sign(EC_KEY const *key, uint8_t const *msg, size_t msglen,
		sig_t *sig)
{
	unsigned int len = 0;

	if (!key || !msg || !sig)
		return (NULL);

	memset(sig->sig, 0, sizeof(sig->sig));
	if (!ECDSA_sign(0, msg, (int)msglen, sig->sig, &len, (EC_KEY *)key))
		return (NULL);

	sig->len = (uint8_t)len;
	return (sig->sig);
}
