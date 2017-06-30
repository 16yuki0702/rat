#ifndef RAT_STRING_H
#define RAT_STRING_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>

typedef struct {
	char *data;
	size_t len;
} rat_str;

#define rat_string(str)		{ (char *)str, sizeof(str) - 1 }
#define rat_null_string(str)	{ NULL, 0 }

extern void rat_str_trim(char *str);

#endif
