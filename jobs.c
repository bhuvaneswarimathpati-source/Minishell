#include "main.h"

void add_job(pid_t pid, int state, char *command)
{
    if (job_count >= MAX_JOBS)
    {
        printf("Maximum number of jobs reached\n");
        return;
    }

    jobs[job_count].job_id = job_count + 1;
    jobs[job_count].pid = pid;
    jobs[job_count].state = state;

    strncpy(jobs[job_count].command,
            command,
            sizeof(jobs[job_count].command) - 1);

    jobs[job_count].command[
        sizeof(jobs[job_count].command) - 1
    ] = '\0';

    job_count++;
}


Job *find_job_by_pid(pid_t pid)
{
    int i;

    for (i = 0; i < job_count; i++)
    {
        if (jobs[i].pid == pid)
            return &jobs[i];
    }

    return NULL;
}


Job *get_last_job(void)
{
    if (job_count == 0)
        return NULL;

    return &jobs[job_count - 1];
}


void remove_job(pid_t pid)
{
    int i;
    int j;

    for (i = 0; i < job_count; i++)
    {
        if (jobs[i].pid == pid)
        {
            for (j = i; j < job_count - 1; j++)
            {
                jobs[j] = jobs[j + 1];
                jobs[j].job_id = j + 1;
            }

            job_count--;

            return;
        }
    }
}


void update_job_status(pid_t pid, int status)
{
    Job *job;

    job = find_job_by_pid(pid);

    if (job == NULL)
        return;

    if (WIFEXITED(status))
    {
        printf("\n[%d]+  Done       %s\n",
               job->job_id,
               job->command);

        remove_job(pid);
    }

    else if (WIFSIGNALED(status))
    {
        printf("\n[%d]+  Terminated %s\n",
               job->job_id,
               job->command);

        remove_job(pid);
    }

    else if (WIFSTOPPED(status))
    {
        job->state = JOB_STOPPED;

        printf("\n[%d]+  Stopped    %s\n",
               job->job_id,
               job->command);
    }

    else if (WIFCONTINUED(status))
    {
        job->state = JOB_RUNNING;
    }

    fflush(stdout);
}


void print_jobs(void)
{
    int i;

    for (i = 0; i < job_count; i++)
    {
        if (jobs[i].state == JOB_RUNNING)
        {
            printf("[%d]+  Running    %s & [%d]\n",
                   jobs[i].job_id,
                   jobs[i].command,
                   jobs[i].pid);
        }
        else
        {
            printf("[%d]+  Stopped    %s [%d]\n",
                   jobs[i].job_id,
                   jobs[i].command,
                   jobs[i].pid);
        }
    }
}


void execute_bg(char *input)
{
    char temp[MAX_INPUT];
    char *arg;
    int job_number = -1;
    Job *job;

    strcpy(temp, input);

    strtok(temp, " \t");

    arg = strtok(NULL, " \t");

    if (arg != NULL)
    {
        job_number = atoi(arg);
    }

    if (job_number == -1)
    {
        job = get_last_job();
    }
    else
    {
        if (job_number <= 0 || job_number > job_count)
        {
            printf("bg: no such job\n");
            return;
        }

        job = &jobs[job_number - 1];
    }

    if (job == NULL)
    {
        printf("bg: no current job\n");
        return;
    }

    if (kill(job->pid, SIGCONT) == -1)
    {
        perror("bg");
        return;
    }

    job->state = JOB_RUNNING;

    printf("[%d]+  Running    %s &\n",
           job->job_id,
           job->command);
}


void execute_fg(char *input)
{
    char temp[MAX_INPUT];
    char *arg;
    int job_number = -1;

    Job *job;
    int status;

    strcpy(temp, input);

    strtok(temp, " \t");

    arg = strtok(NULL, " \t");

    if (arg != NULL)
    {
        job_number = atoi(arg);
    }

    if (job_number == -1)
    {
        job = get_last_job();
    }
    else
    {
        if (job_number <= 0 || job_number > job_count)
        {
            printf("fg: no such job\n");
            return;
        }

        job = &jobs[job_number - 1];
    }

    if (job == NULL)
    {
        printf("fg: no current job\n");
        return;
    }

    foreground_pid = job->pid;

    if (kill(job->pid, SIGCONT) == -1)
    {
        perror("fg");
        foreground_pid = 0;
        return;
    }

    job->state = JOB_RUNNING;

    printf("%s\n", job->command);

    while (1)
    {
        pid_t ret;

        ret = waitpid(job->pid, &status, WUNTRACED);

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
        remove_job(job->pid);
    }
    else if (WIFSIGNALED(status))
    {
        last_exit_status = 128 + WTERMSIG(status);
        remove_job(job->pid);
    }
    else if (WIFSTOPPED(status))
    {
        job->state = JOB_STOPPED;

        printf("\n[%d]+  Stopped    %s\n",
               job->job_id,
               job->command);

        last_exit_status = 128 + WSTOPSIG(status);
    }
}