#include "cli.h"

/**
 * find_command - looks up a CLI command by its name
 *
 * @name: name typed by the user (e.g. "send", "wallet_load")
 *
 * Return: pointer to the matching command, or NULL if there is none
 */
command_t const *find_command(char const *name)
{
	command_t const *command = get_commands();

	if (!name)
		return (NULL);
	for (; command->name; command++)
	{
		if (strcmp(command->name, name) == 0)
			return (command);
	}
	return (NULL);
}
