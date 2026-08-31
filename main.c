/*Name: Bhuvaneshwari
  Project Name: Minishell project.
  date:1/09/2026.
  Batch Id:int26001B_174
  description: Mini Shell (msh) is a basic Linux command-line shell developed using C. 
  It executes built-in and external commands using system calls such as fork(), execvp(), and waitpid(). It supports customizable prompts, special variables, signal handling (Ctrl+C, Ctrl+Z), background processes, job control (jobs, fg, bg), and pipe operations for inter-process communication.

Objective:

To understand and implement Linux system calls, process creation, process synchronization, signal handling, command parsing, exit status, background processing, and inter-process communication using pipes.*/




#include "main.h"

pid_t shell_pid;
pid_t foreground_pid = 0;
int last_exit_status = 0;

char shell_path[1024];

Job jobs[MAX_JOBS];
int job_count = 0;

int main()
{
    char input[MAX_INPUT];
    char *ps1;

    shell_pid = getpid();

    /* Get path of msh */
    if (readlink("/proc/self/exe", shell_path, sizeof(shell_path) - 1) == -1)
    {
        strcpy(shell_path, "./msh");
    }
    else
    {
        shell_path[sizeof(shell_path) - 1] = '\0';
    }

    setup_signals();

    while (1)
    {
        /* Check background processes */
        int status;
        pid_t pid;

        while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0)
        {
            if (pid != foreground_pid)
            {
                update_job_status(pid, status);
            }
        }

        ps1 = getenv("PS1");

        if (ps1 == NULL)
            printf("Minishell$ ");
        else
            printf("%s", ps1);

        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        trim_spaces(input);

        /* Empty command */
        if (strlen(input) == 0)
            continue;

        /* Expand $? $$ $SHELL */
        expand_special_variables(input);

        execute_command(input);
    }

    return 0;
}