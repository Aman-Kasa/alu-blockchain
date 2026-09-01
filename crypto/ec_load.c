#include <stdio.h>
#include "hblk_crypto.h"

/**
 * ec_load - Loads an EC key pair from the disk
 * @folder: Path to the folder from which to load the keys
 *
 * Return: Pointer to created EC_KEY structure, or NULL upon failure
 */
EC_KEY *ec_load(char const *folder)
{
	char path[512];
	FILE *fp = NULL;
	EC_KEY *key = NULL;

	if (!folder)
		return (NULL);

	snprintf(path, sizeof(path), "%s/" PRI_FILENAME, folder);
	fp = fopen(path, "r");
	if (fp)
	{
		key = PEM_read_ECPrivateKey(fp, NULL, NULL, NULL);
		fclose(fp);
		if (key)
			return (key);
	}

	snprintf(path, sizeof(path), "%s/" PUB_FILENAME, folder);
	fp = fopen(path, "r");
	if (fp)
	{
		key = PEM_read_EC_PUBKEY(fp, NULL, NULL, NULL);
		fclose(fp);
	}

	return (key);
}
