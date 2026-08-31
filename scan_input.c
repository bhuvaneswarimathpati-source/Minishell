#include "main.h"

void trim_spaces(char *str)
{
    int start = 0;
    int end;
    int i;

    while (str[start] == ' ' || str[start] == '\t')
        start++;

    end = strlen(str) - 1;

    while (end >= start &&
           (str[end] == ' ' || str[end] == '\t'))
    {
        end--;
    }

    for (i = start; i <= end; i++)
        str[i - start] = str[i];

    str[i - start] = '\0';
}


/* Replace special variables */
void expand_special_variables(char *input)
{
    char result[MAX_INPUT];
    char temp[100];
    int i = 0;
    int j = 0;

    result[0] = '\0';

    while (input[i] != '\0' && j < MAX_INPUT - 1)
    {
        if (input[i] == '$')
        {
            /* $? */
            if (input[i + 1] == '?')
            {
                sprintf(temp, "%d", last_exit_status);
                strcat(result, temp);

                i += 2;
                j = strlen(result);
                continue;
            }

            /* $$ */
            if (input[i + 1] == '$')
            {
                sprintf(temp, "%d", shell_pid);
                strcat(result, temp);

                i += 2;
                j = strlen(result);
                continue;
            }

            /* $SHELL */
            if (strncmp(&input[i], "$SHELL", 6) == 0)
            {
                strcat(result, shell_path);

                i += 6;
                j = strlen(result);
                continue;
            }
        }

        result[j++] = input[i++];
        result[j] = '\0';
    }

    strcpy(input, result);
}