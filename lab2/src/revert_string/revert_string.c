#include <string.h>
#include "revert_string.h"

#define SWAP(a, b) {a^=b;b^=a;a^=b;};

void RevertString(char *str)
{
	size_t len = strlen(str);
	for (size_t i = 0; i < len / 2; i++)
	{
		SWAP(str[i], str[len - i - 1]);
	}
}

