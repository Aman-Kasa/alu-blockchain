#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/**
 * _print_hex_buffer - Prints a buffer of bytes in hexadecimal format
 * @buf: Buffer to print
 * @len: Length of buffer
 */
void _print_hex_buffer(uint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; buf && i < len; i++)
		printf("%02x", buf[i]);
}
