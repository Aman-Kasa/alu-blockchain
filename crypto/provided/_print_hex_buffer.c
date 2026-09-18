#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

/**
* _print_hex_buffer - Prints a buffer in its hexadecimal form.
* @buf: Pointer to the buffer to be printed.
* @len: Number of bytes from @buf to be printed.
* Author: Frank Onyema Orji.
 */

void _print_hex_buffer(unint8_t const *buf, size_t len)
{
	size_t i;

	for (i = 0; buff && i < len; i++)
		printf("%02x", buff[i]);
}
