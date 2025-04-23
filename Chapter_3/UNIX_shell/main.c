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
                    /* When the process enter this section, it means that first command has been
                     * read and stored in args while the second command is left in stdin. '|' is
                     * skipped. */
                    int fd[2];
                    if (pipe (fd) == -1)
                        perror ("pipe failed"), exit (1);
                    pid_t ppid = fork ();
                    if (ppid == -1)
                        perror ("child:failed to fork"), exit (1);
                    if (ppid == 0)
                        {
                            /* Child of child process, handling the first command and redirect
                             * output. */
                            close (fd[0]);
                            if (dup2 (fd[1], STDOUT_FILENO) == -1)
                                perror ("child-child: failed to dup2"), exit (1);
                            execvp (args[0], args);
                            perror ("child-child: failed to execute");
                        }
                    else
                        {
                            /* Parent of child-child process, handling the second command and
                             * redirect input. */
                            close (fd[1]);
                            wait (NULL);
                            read_command (args);
                            if (dup2 (fd[0], STDIN_FILENO) == -1)
                                perror ("child: failed to dup2"), exit (1);
                            int arg_offset = count_offset (args);
                            read_command (args + arg_offset);
                            execvp (args[0], args);
                            perror ("child: failed to execute");
                        }
                    exit (1); // These two process have to exit despite the exectue failure.
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
            reset_flags ();

            /* Read user inputs and set flags. */
            read_command (args);
            if (args[0] == NULL)
                {
                    perror ("No commands in history");
                    continue;
                }
            if (flag_exit)
                break;
            exe_command (args);
        }
    int i;
    for (i = 0; i <= MAX_LINE / 2; ++i)
        free (args[i]);
    return 0;
}
