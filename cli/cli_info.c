#include "cli.h"

/**
 * sum_unspent_amounts - adds one unspent output to the running totals
 *
 * @node: a pointer to the unspent_tx_out_t structure
 * @idx: the index of the current item in the list (unused)
 * @arg: a pointer to the coins_t accumulator
 *
 * Return: 0 (keep iterating)
 */
static int sum_unspent_amounts(void *node,
			       unsigned int idx __attribute__((unused)), void *arg)
{
	unspent_tx_out_t *unspent_tx_out = node;
	coins_t *coins = arg;

	coins->total += unspent_tx_out->out.amount;
	if (!memcmp(unspent_tx_out->out.pub, coins->pub, EC_PUB_LEN))
		coins->mine += unspent_tx_out->out.amount;

	return (0);
}

/**
 * cli_info - prints a summary of the blockchain and of the wallet
 *
 * Description: displays the number of blocks, of unspent transaction
 * outputs and of transactions waiting in the pool, the total number of
 * coins in circulation, and the balance of the current wallet
 *
 * @state: a pointer to the state_t structure
 *         containing all blockchain information
 *
 * Return: EXIT_SUCCESS on successful execution,
 *         or 2 if there are too many arguments
 */
int cli_info(state_t *state)
{
	coins_t coins;

	if (state->argc > 1)
	{
		fprintf(stderr, "%s: too many arguments\n", state->argv[0]);
		return ((state->status = 2));
	}
	memset(&coins, 0, sizeof(coins));
	if (state->wallet)
		ec_to_pub(state->wallet, coins.pub);
	llist_for_each(state->blockchain->unspent, sum_unspent_amounts, &coins);

	fprintf(stdout, "Blocks:  %d\n",
		llist_size(state->blockchain->chain));
	fprintf(stdout, "UTXOs:   %d\n",
		llist_size(state->blockchain->unspent));
	fprintf(stdout, "TX Pool: %d\n",
		llist_size(state->tx_pool));
	fprintf(stdout, "Coins:   %lu\n", coins.total);
	fprintf(stdout, "Balance: %lu\n", coins.mine);

	return ((state->status = EXIT_SUCCESS));
}
