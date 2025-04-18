#include "uheads.h"

/* Output current args text. */
void repeat_command (char *args[])
{
    int k;
    for (k = 0; args[k + 1] != NULL; ++k)
        printf ("%s ", args[k]);
    printf ("%s\n", args[k]);
}

/* Invoked after executing. Must be run by parent process because it holds the flags. */
void reset_flags ()
{
    flag_pipe = 0;
    flag_wait = 1;
    flag_repe = 0;

    /* Restore the meaning of std file descriptors. */
    if (infd_backup)
        dup2 (infd_backup, STDIN_FILENO), close (infd_backup), infd_backup = 0;
    if (outfd_backup)
        dup2 (outfd_backup, STDOUT_FILENO), close (outfd_backup), outfd_backup = 0;
}

/* Return the index of first NULL in *args[]. */
int count_offset (char *args[])
{
    int i = 0;
    while (args[i] != NULL)
        ++i;
    return i;
}

/* This function executes the commands in child process according to flags. */
void exe_command (char *args[])
{
    pid_t pid = fork ();
    if (pid == -1)
        {
            perror ("failed to frok");
            return;
        }
    if (pid == 0)
        {
            if (flag_pipe)
                {
                    int arg_offset = count_offset (args);
                    read_command (args + arg_offset);
                    exe_command (args + arg_offset);
                    exit (0); // The child process exits.
                }
            if (flag_repe)
                repeat_command (args);
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
    flag_exit = 0;                           // The flag of main loop to break.

    while (!flag_exit)
        {
            printf ("osh>");
            fflush (stdout);

            /* Read user inputs. */
            read_command (args);
            if (args[0] == NULL)
                {
                    perror ("No commands in history");
                    continue;
                }
            if (!flag_exit)
                {
                    exe_command (args);
                    reset_flags ();
                }
        }
    int i;
    for (i = 0; i <= MAX_LINE / 2; ++i)
        free (args[i]);
    return 0;
}
