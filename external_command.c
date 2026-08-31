#include "main.h"

void execute_external(char *input)
{
    char temp[MAX_INPUT];
    char *args[MAX_ARGS];
    char *token;

    int argc = 0;
    int background = 0;

    pid_t pid;
    int status;

    strcpy(temp, input);

    trim_spaces(temp);

    /* Check & */
    int len = strlen(temp);

    if (len > 0 && temp[len - 1] == '&')
    {
        background = 1;
        temp[len - 1] = '\0';

        trim_spaces(temp);
    }

    /* Tokenize */
    token = strtok(temp, " \t");

    while (token != NULL && argc < MAX_ARGS - 1)
    {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }

    args[argc] = NULL;

    if (argc == 0)
        return;

    /* PS1=NEW_PROMPT is a builtin-like assignment */
    if (strncmp(args[0], "PS1=", 4) == 0)
    {
        setenv("PS1", args[0] + 4, 1);
        last_exit_status = 0;
        return;
    }

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        last_exit_status = 1;
        return;
    }

    if (pid == 0)
    {
        /* Child */

        /* Restore default signals */
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

        execvp(args[0], args);

        perror(args[0]);
        exit(127);
    }

    /* Parent */

    if (background)
    {
        add_job(pid, JOB_RUNNING, temp);

        printf("[%d] %d\n",
               jobs[job_count - 1].job_id,
               pid);

        last_exit_status = 0;
        return;
    }

    foreground_pid = pid;

    while (1)
    {
        pid_t ret;

        ret = waitpid(pid, &status, WUNTRACED);

        if (ret == -1)
        {
            if (errno == EINTR)
                continue;

            perror("waitpid");
            break;
        }

        break;
    }

    foreground_pid = 0;

    if (WIFEXITED(status))
    {
        last_exit_status = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        last_exit_status = 128 + WTERMSIG(status);
    }
    else if (WIFSTOPPED(status))
    {
        add_job(pid, JOB_STOPPED, temp);

        printf("\n[%d]+  Stopped    %d\n",
               jobs[job_count - 1].job_id,
               pid);

        last_exit_status = 128 + WSTOPSIG(status);
    }
}