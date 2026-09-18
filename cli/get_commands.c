#include "cli.h"

/**
 * get_commands - program that retrieves a list of command structures
 *
 * each command structure is defined with specific handlers, names,
 * help texts, and descriptions
 *
 * Return: a pointer to an array of command_t structures;
 *         the array is terminated by a NULL entry
 */

command_t const *get_commands(void)
{
	static command_t commands[] = {
		{cli_exit, EXIT_NAME, EXIT_HELP, EXIT_DESC},
		{cli_help, HELP_NAME, HELP_HELP, HELP_DESC},
		{cli_info, INFO_NAME, INFO_HELP, INFO_DESC},
		{cli_load, LOAD_NAME, LOAD_HELP, LOAD_DESC},
		{cli_mine, MINE_NAME, MINE_HELP, MINE_DESC},
		{cli_save, SAVE_NAME, SAVE_HELP, SAVE_DESC},
		{cli_send, SEND_NAME, SEND_HELP, SEND_DESC},
		{cli_wallet_load, WALLET_LOAD_NAME, WALLET_LOAD_HELP, WALLET_LOAD_DESC},
		{cli_wallet_save, WALLET_SAVE_NAME, WALLET_SAVE_HELP, WALLET_SAVE_DESC},
		{NULL, NULL, NULL, NULL}
	};

	return (commands);
}

