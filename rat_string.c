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
