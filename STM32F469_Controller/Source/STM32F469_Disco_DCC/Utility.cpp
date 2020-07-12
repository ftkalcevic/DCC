#include "Utility.h"
#include <string.h>

char16_t *strdup16(const char *src)
{
	int len = strlen(src);
	char16_t *dest = new char16_t[len + 1];
	for (int i = 0; i < len+1; i++)		// +1 to include null terminator
		dest[i] = src[i];
	return dest;
}

char16_t *strncpy16(char16_t *dest, const char *src, int num)
{
	int len = strlen(src)+1;			// +1 to include null terminator
	if (len > num)
		len = num;
	for (int i = 0; i < len; i++)
		dest[i] = src[i];
	return dest;
}
