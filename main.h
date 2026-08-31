#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define BUILTIN      1
#define EXTERNAL     2
#define NO_COMMAND   3

#define MAX_INPUT    1024
#define MAX_ARGS     100
#define MAX_JOBS     100

/* Job states */
#define JOB_RUNNING  1
#define JOB_STOPPED  2

/* Global shell information */
extern pid_t shell_pid;
extern pid_t foreground_pid;
extern int last_exit_status;
extern char shell_path[1024];

/* Job structure */
typedef struct
{
    int job_id;
    pid_t pid;
    int state;
    char command[1024];
} Job;

/* Job table */
extern Job jobs[MAX_JOBS];
extern int job_count;

/* Input */
void scan_input(char *input);

/* Command checking */
int check_command_type(char *command);

/* Execution */
void execute_command(char *input);

/* Builtins */
int execute_builtin(char *input);

/* Pipes */
void execute_pipe(char *input);

/* Signals */
void signal_handler(int sig);
void setup_signals(void);

/* Jobs */
void add_job(pid_t pid, int state, char *command);
void remove_job(pid_t pid);
void update_job_status(pid_t pid, int status);
void print_jobs(void);
void execute_bg(char *input);
void execute_fg(char *input);
Job *get_last_job(void);
Job *find_job_by_pid(pid_t pid);

/* External command */
void execute_external(char *input);

/* Utility */
void trim_spaces(char *str);
void expand_special_variables(char *input);

#endif