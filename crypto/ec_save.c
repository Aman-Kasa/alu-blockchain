#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include "hblk_crypto.h"

/**
 * ec_save - Saves an existing EC key pair on disk in PEM format
 * @key: Pointer to the EC_KEY structure to save
 * @folder: Path to folder in which to save the keys
 *
 * Return: 1 upon success, 0 upon failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	char path[512];
	FILE *fp = NULL;

	if (!key || !folder)
		return (0);

	mkdir(folder, 0755);

	snprintf(path, sizeof(path), "%s/key.pem", folder);
	fp = fopen(path, "w");
	if (!fp)
		return (0);

	if (!PEM_write_ECPrivateKey(fp, key, NULL, NULL, 0, NULL, NULL))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	snprintf(path, sizeof(path), "%s/key_pub.pem", folder);
	fp = fopen(path, "w");
	if (!fp)
		return (0);

	if (!PEM_write_EC_PUBKEY(fp, key))
	{
		fclose(fp);
		return (0);
	}
	fclose(fp);

	return (1);
}
