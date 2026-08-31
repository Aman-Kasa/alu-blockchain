#include "hblk_crypto.h"

/**
 * ec_save - Saves an existing EC key pair on disk
 * @key: Pointer to the EC key pair to be saved
 * @folder: Path to the folder in which to save the keys
 *
 * Return: 1 upon success, or 0 upon failure
 */
int ec_save(EC_KEY *key, char const *folder)
{
	char path[512];
	FILE *fp = NULL;
	struct stat st = {0};

	if (!key || !folder)
		return (0);

	if (stat(folder, &st) == -1)
	{
		if (mkdir(folder, 0777) != 0)
			return (0);
	}

	/* Save private key */
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

	/* Save public key */
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
