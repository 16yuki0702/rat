#include "rat_string.h"

void
rat_str_trim(rat_str *str)
{
        int i;
        if (str == NULL) {
                return;
        }

        i = strlen(str->data);
        while (--i >= 0 && str->data[i] == ' ');

        str->data[i + 1] = '\0';

        i = 0;
        while (str->data[i] != '\0' && str->data[i] == ' ') i++;

        strcpy(str->data, &str->data[i]);
	str->len = strlen(str->data);
}
