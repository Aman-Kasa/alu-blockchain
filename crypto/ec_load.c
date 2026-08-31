#include "hblk_crypto.h"

/**
 * ec_load - Loads an EC key pair from disk
 * @folder: Path to the folder from which to load the keys
 *
 * Return: Pointer to the created EC key pair upon success, or NULL upon failure
 */
EC_KEY *ec_load(char const *folder)
{
	char path[512];
	FILE *fp = NULL;
	EC_KEY *key = NULL;

	if (!folder)
		return (NULL);

	snprintf(path, sizeof(path), "%s/key.pem", folder);
	fp = fopen(path, "r");
	if (fp)
	{
		key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
		fclose(fp);
	}

	if (!key)
	{
		snprintf(path, sizeof(path), "%s/key_pub.pem", folder);
		fp = fopen(path, "r");
		if (fp)
		{
			key = PEM_read_EC_PUBKEY(fp, NULL, NULL, NULL);
			fclose(fp);
		}
	}

	return (key);
}
