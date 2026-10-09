#include "cli.h"

/**
 * cli_address - prints the public address of the current wallet
 *
 * @state: CLI state holding the wallet
 *
 * Description: the address is the uncompressed public key, printed as
 * 130 hex characters. Give it to someone so they can `send` you coins.
 *
 * Return: EXIT_SUCCESS, or EXIT_FAILURE if there is no usable wallet
 */
int cli_address(state_t *state)
{
	uint8_t pub[EC_PUB_LEN];
	size_t i;

	if (!state->wallet || !ec_to_pub(state->wallet, pub))
	{
		fprintf(stderr, "%s: no wallet loaded\n", state->argv[0]);
		return ((state->status = EXIT_FAILURE));
	}
	for (i = 0; i < EC_PUB_LEN; i++)
		printf("%02x", pub[i]);
	printf("\n");
	return ((state->status = EXIT_SUCCESS));
}
