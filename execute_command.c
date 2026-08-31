#include "main.h"

void execute_command(char *input)
{
    char temp[MAX_INPUT];

    strcpy(temp, input);
    trim_spaces(temp);

    if (strlen(temp) == 0)
        return;

    /* Pipe command */
    if (strchr(temp, '|') != NULL)
    {
        execute_pipe(temp);
        return;
    }

    /* Built-in */
    if (check_command_type(temp) == BUILTIN)
    {
        last_exit_status = execute_builtin(temp);
        return;
    }

    /* Background command */
    int len = strlen(temp);

    if (len > 0 && temp[len - 1] == '&')
    {
        execute_external(temp);
        return;
    }

    /* External */
    execute_external(temp);
}