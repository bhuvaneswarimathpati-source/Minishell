#include "main.h"

int execute_builtin(char *input)
{
    char temp[MAX_INPUT];
    char *args[MAX_ARGS];
    char *token;
    int argc = 0;

    strcpy(temp, input);

    token = strtok(temp, " \t");

    while (token != NULL && argc < MAX_ARGS - 1)
    {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }

    args[argc] = NULL;

    if (argc == 0)
        return 0;

    /* exit */
    if (strcmp(args[0], "exit") == 0)
    {
        if (job_count > 0)
        {
            printf("There are stopped/background jobs.\n");
            return 1;
        }

        exit(0);
    }

    /* PS1=NEW_PROMPT */
    if (strncmp(args[0], "PS1=", 4) == 0)
    {
        char *value = args[0] + 4;

        setenv("PS1", value, 1);

        return 0;
    }

    /* cd */
    if (strcmp(args[0], "cd") == 0)
    {
        char *path;

        if (args[1] == NULL)
        {
            path = getenv("HOME");

            if (path == NULL)
                path = "/";
        }
        else
        {
            path = args[1];
        }

        if (chdir(path) == -1)
        {
            perror("cd");
            return 1;
        }

        return 0;
    }

    /* pwd */
    if (strcmp(args[0], "pwd") == 0)
    {
        char cwd[1024];

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("pwd");
            return 1;
        }

        printf("%s\n", cwd);

        return 0;
    }

    /* jobs */
    if (strcmp(args[0], "jobs") == 0)
    {
        print_jobs();
        return 0;
    }

    /* bg */
    if (strcmp(args[0], "bg") == 0)
    {
        execute_bg(input);
        return 0;
    }

    /* fg */
    if (strcmp(args[0], "fg") == 0)
    {
        execute_fg(input);
        return 0;
    }

    return 0;
}