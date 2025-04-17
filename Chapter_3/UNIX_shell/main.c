#include <ctype.h> // It includes type judge functions.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80

int read_command (char *args[])
{
    char buffer[MAX_LINE];
    int i, j;
    int end_of_line = 0;
    static int flag_wait;
    for (i = 0; i <= MAX_LINE / 2; ++i)
        {
            /* Read a single word. */
            for (j = 0; j < MAX_LINE; ++j)
                {
                    buffer[j] = getchar ();
                    switch (buffer[j])
                        {
                        case '\n':
                            end_of_line = 1; // It goes to next case then.
                        case ' ':
                            buffer[j] = 0;
                            break;
                        case '!':
                            if (j > 0 && buffer[j - 1] == '!')
                                {
                                    while (buffer[j] != '\n')
                                        buffer[j] = getchar ();
                                    return flag_wait;
                                } // Initiation doesn't matter.
                        default:
                            continue;
                        }
                    if (buffer[j] == 0)
                        break;
                }
            /* Store the argument. */
            args[i] = (char *)malloc (strlen (buffer));
            strcpy (args[i], buffer);

            if (end_of_line)
                break;
        }

    /* Set the last argument NULL to avoid misreading the previous. */
    free (args[i + 1]);
    args[i + 1] = NULL;

    /* Check '&' signal. */
    if (strcmp (args[i], "&") == 0)
        {
            free (args[i]);
            args[i] = NULL;
            return flag_wait = 0;
        }
    else
        return flag_wait = 1;
}

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
        }
    int i;
    for (i = 0; i <= MAX_LINE / 2; ++i)
        free (args[i]);
    return 0;
}
