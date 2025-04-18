#include "uheads.h"

/* This function execute the command args[0] in child process and decide whether the parent
 * process need to wait arroding to flag. */
void exe_command (char *args[], int flag_wait)
{
    pid_t pid = fork ();
    if (pid == -1)
        perror ("failed to frok");
    else if (pid == 0)
        {
            execvp (args[0], args);
            perror ("failed to execute");
            exit (1);
        }
    else if (flag_wait)
        wait (NULL);
}

int main (void)
{
    char *args[MAX_LINE / 2 + 1] = { NULL }; // An array of pointers that saves command arguments.
    int should_run = 1;                      // The flag of main loop to continue.
    int wait_or_not;                         // The flag to conduct parent process to wait.

    while (should_run)
        {
            printf ("osh>");
            fflush (stdout);

            /* Read user inputs. */
            wait_or_not = read_command (args);
            if (args[0] == NULL)
                {
                    fprintf (stderr, "No commands in history.\n");
                    continue;
                }

            /* Follow the commands in args. */
            if (strcmp (args[0], "exit") == 0)
                {
                    should_run = 0;
                    continue;
                }
            exe_command (args, wait_or_not);

            /* Restore the meaning of std file descriptors. */
            if (infd_backup)
                dup2 (infd_backup, STDIN_FILENO), close (infd_backup), infd_backup = 0;
            if (outfd_backup)
                dup2 (outfd_backup, STDOUT_FILENO), close (outfd_backup), outfd_backup = 0;
        }
    int i;
    for (i = 0; i <= MAX_LINE / 2; ++i)
        free (args[i]);
    return 0;
}
