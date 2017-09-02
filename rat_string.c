#include "rat_string.h"

void
rat_str_trim(char *str)
{
        int i;

        if (str == NULL) {
                return;
        }

        i = strlen(str);

        while (--i >= 0 && str[i] == ' ');

        str[i + 1] = '\0';

        i = 0;
        while (str[i] != '\0' && str[i] == ' ') i++;
        strcpy(str, &str[i]);
}

rat_str *
make_rat_str(char *str)
{
	rat_str *r;

	r = (rat_str*)malloc(sizeof(rat_str));
	memset(r, 0, sizeof(r));

	r->data = (char*)malloc(sizeof(char) * strlen(str));
	strcpy(r->data, str);

	if (r->data[strlen(r->data) - 1] == '\n') {
		r->data[strlen(r->data) - 1] = '\0';
	}

	r->len = strlen(str);

	return r;
}

rat_str *
make_rat_strn(char *str, size_t size)
{
	rat_str *r;

	r = (rat_str*)malloc(sizeof(rat_str));
	memset(r, 0, sizeof(r));

	r->data = (char*)malloc(sizeof(char) * size);
	strncpy(r->data, str, size);

	r->len = size;

	return r;
}
