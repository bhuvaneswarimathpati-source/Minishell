#include "main.h"

void execute_pipe(char *input)
{
    char temp[MAX_INPUT];

    char *commands[MAX_ARGS];

    int pipe_count = 0;
    int command_count;

    int pipes[MAX_ARGS][2];

    pid_t pids[MAX_ARGS];

    char *token;

    int i;
    int status;

    strcpy(temp, input);

    /*
     * Split commands using |
     */
    token = strtok(temp, "|");

    while (token != NULL && pipe_count < MAX_ARGS - 1)
    {
        commands[pipe_count] = token;

        trim_spaces(commands[pipe_count]);

        pipe_count++;

        token = strtok(NULL, "|");
    }

    command_count = pipe_count;

    if (command_count <= 0)
        return;

    /*
     * Create required pipes.
     *
     * n commands need n-1 pipes.
     */
    for (i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            return;
        }
    }

    /*
     * Create children
     */
    for (i = 0; i < command_count; i++)
    {
        char command_copy[MAX_INPUT];
        char *args[MAX_ARGS];
        char *arg;
        int argc = 0;

        strcpy(command_copy, commands[i]);

        arg = strtok(command_copy, " \t");

        while (arg != NULL && argc < MAX_ARGS - 1)
        {
            args[argc++] = arg;
            arg = strtok(NULL, " \t");
        }

        args[argc] = NULL;

        if (argc == 0)
            continue;

        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            return;
        }

        if (pids[i] == 0)
        {
            /*
             * Restore default signals in child
             */
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);

            /*
             * First command:
             *
             * stdin  = normal
             * stdout = pipe
             */
            if (i == 0)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            /*
             * Last command:
             *
             * stdin = previous pipe
             * stdout = normal
             */
            else if (i == command_count - 1)
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            /*
             * Middle command:
             *
             * stdin  = previous pipe
             * stdout = next pipe
             */
            else
            {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }

                if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }

            /*
             * Close all pipe descriptors
             */
            int j;

            for (j = 0; j < command_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(args[0], args);

            perror(args[0]);
            exit(127);
        }
    }

    /*
     * Parent closes all pipes
     */
    for (i = 0; i < command_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /*
     * Parent waits for all children
     */
    foreground_pid = pids[command_count - 1];

    for (i = 0; i < command_count; i++)
    {
        waitpid(pids[i], &status, WUNTRACED);
    }

    foreground_pid = 0;

    /*
     * Exit status of pipeline = last command status
     */
    if (WIFEXITED(status))
    {
        last_exit_status = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        last_exit_status = 128 + WTERMSIG(status);
    }
}