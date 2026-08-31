#include "main.h"

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        if (foreground_pid > 0)
        {
            kill(foreground_pid, SIGINT);
        }
        else
        {
            printf("\n");
            fflush(stdout);
        }
    }

    else if (sig == SIGTSTP)
    {
        if (foreground_pid > 0)
        {
            kill(foreground_pid, SIGTSTP);
        }
        else
        {
            printf("\n");
            fflush(stdout);
        }
    }

    else if (sig == SIGCHLD)
    {
        /* Do not perform waitpid here.
           Main loop performs non-blocking cleanup. */
    }
}


void setup_signals(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = signal_handler;

    sigemptyset(&sa.sa_mask);

    /* Ctrl-C */
    sigaction(SIGINT, &sa, NULL);

    /* Ctrl-Z */
    sigaction(SIGTSTP, &sa, NULL);

    /* Background child */
    sigaction(SIGCHLD, &sa, NULL);

    /*
     * Shell should ignore these terminal signals.
     */
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}