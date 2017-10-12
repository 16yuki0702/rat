#ifndef RAT_STRING_H
#define RAT_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct {
	char *data;
	size_t len;
} rat_str;

typedef struct {
	char *d;
	size_t l;
} r_str;

#define rat_string(str)		({ (char *)str, sizeof(str) - 1 })
#define rat_null_string(str)	({ NULL, 0 })
#define RAT_STR_CPY(c1, c2)						\
	strncpy(c1, c2, strlen(c2));					\
	c1[strlen(c2)] = '\0';
#define DUMP_STR(s)							\
	do {								\
		int ii;							\
		for (ii = 0; ii < s.l; ii++) {				\
			printf("%c", s.d[ii]);				\
		}							\
		printf("\n");						\
	} while (0)


extern void rat_str_trim(char *str);

extern rat_str *make_rat_str(char *);
extern rat_str *make_rat_strn(char *, size_t);

#endif
