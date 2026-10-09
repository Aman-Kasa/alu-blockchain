#include "cli.h"

/**
 * parse_amount - converts a command-line argument into a coin amount
 *
 * @str: the argument typed by the user
 * @amount: where to store the parsed amount
 *
 * Return: 1 if @str is a valid amount between 1 and UINT32_MAX, 0 otherwise
 */
static int parse_amount(char const *str, uint32_t *amount)
{
	unsigned long int value;
	char *endptr = NULL;

	if (*str == '\0' || *str == '-')
		return (0);
	errno = 0;
	value = strtoul(str, &endptr, 0);
	if (errno != 0 || *endptr != '\0' || value == 0 || value > UINT32_MAX)
		return (0);
	*amount = (uint32_t)value;
	return (1);
}

/**
 * parse_address - converts a hex address into a raw public key
 *
 * @hex: the address typed by the user (130 hex characters)
 * @pub: where to store the 65 bytes of the public key
 *
 * Description: each pair of hex characters is read into a temporary
 * unsigned int, then stored as one byte, so nothing is ever written
 * past the end of @pub
 *
 * Return: 1 on success, 0 if @hex is not a valid address
 */
static int parse_address(char const *hex, uint8_t pub[EC_PUB_LEN])
{
	unsigned int byte;
	size_t i;

	if (strlen(hex) != 2 * EC_PUB_LEN)
		return (0);
	for (i = 0; i < EC_PUB_LEN; i++)
	{
		if (!isxdigit((unsigned char)hex[2 * i]) ||
			!isxdigit((unsigned char)hex[2 * i + 1]) ||
			sscanf(hex + 2 * i, "%2x", &byte) != 1)
			return (0);
		pub[i] = (uint8_t)byte;
	}
	return (1);
}

/**
 * queue_transaction - creates a transaction and adds it to the local pool
 *
 * @state: the CLI state (wallet, blockchain and transaction pool)
 * @amount: the number of coins to send
 * @pub: the public key of the receiver
 *
 * Return: EXIT_SUCCESS if the transaction was added to the pool,
 *         EXIT_FAILURE otherwise
 */
static int queue_transaction(state_t *state, uint32_t amount,
	uint8_t pub[EC_PUB_LEN])
{
	EC_KEY *receiver = ec_from_pub(pub);
	transaction_t *tx = NULL;
	char const *error = NULL;

	if (!receiver)
		error = "invalid receiver public key";
	else
		tx = transaction_create(state->wallet, receiver, amount,
			state->blockchain->unspent);
	if (!error && !tx)
		error = "failed to create transaction (not enough coins?)";
	else if (!error && !transaction_is_valid(tx, state->blockchain->unspent))
		error = "invalid transaction";
	else if (!error && llist_add_node(state->tx_pool, tx, ADD_NODE_REAR) == -1)
		error = "failed to add transaction to local transaction pool";
	EC_KEY_free(receiver);
	if (error)
	{
		fprintf(stderr, "%s: %s\n", state->argv[0], error);
		transaction_destroy(tx);
		return ((state->status = EXIT_FAILURE));
	}
	fprintf(stdout, "Transaction added to local transaction pool\n");
	return ((state->status = EXIT_SUCCESS));
}

/**
 * cli_send - sends coins from the current wallet to an address
 *
 * Description: usage is `send AMOUNT ADDRESS`. The transaction is put in
 * the local transaction pool and confirmed by the next `mine`.
 *
 * @state: the current CLI state including arguments from the command line
 *
 * Return: 2 on argument count errors, EXIT_FAILURE on invalid input or
 *         failed transaction, EXIT_SUCCESS otherwise
 */
int cli_send(state_t *state)
{
	uint8_t pub[EC_PUB_LEN] = {0};
	uint32_t amount = 0;

	if (state->argc != 3)
	{
		fprintf(stderr, "%s: too %s arguments\n", state->argv[0],
			state->argc > 3 ? "many" : "few");
		return ((state->status = 2));
	}
	if (!parse_amount(state->argv[1], &amount))
	{
		fprintf(stderr, "%s: %s: invalid amount\n",
			state->argv[0], state->argv[1]);
		return ((state->status = EXIT_FAILURE));
	}
	if (!parse_address(state->argv[2], pub))
	{
		fprintf(stderr, "%s: invalid address (expected %d hex characters)\n",
			state->argv[0], 2 * EC_PUB_LEN);
		return ((state->status = EXIT_FAILURE));
	}
	return (queue_transaction(state, amount, pub));
}
