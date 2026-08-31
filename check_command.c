#include "main.h"

int check_command_type(char *command)
{
    char temp[MAX_INPUT];
    char first_word[100];

    strcpy(temp, command);

    trim_spaces(temp);

    if (strlen(temp) == 0)
        return NO_COMMAND;

    sscanf(temp, "%99s", first_word);

    /* Built-in commands */
    if (strcmp(first_word, "exit") == 0 ||
        strcmp(first_word, "cd") == 0 ||
        strcmp(first_word, "pwd") == 0 ||
        strcmp(first_word, "jobs") == 0 ||
        strcmp(first_word, "fg") == 0 ||
        strcmp(first_word, "bg") == 0)
    {
        return BUILTIN;
    }

    return EXTERNAL;
}