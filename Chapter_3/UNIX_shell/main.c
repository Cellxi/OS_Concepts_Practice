#include <ctype.h> // It includes type judge functions.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80

void child (char *args[]) {}
int read_command (char *args[])
{
    char buffer[MAX_LINE];
    int i, j;
    int end_of_line = 0;
    for (i = 1; i <= MAX_LINE / 2; ++i)
        {
            /* Read a single word. */
            for (j = 0; j < MAX_LINE; ++j)
                {
                    buffer[j] = getchar ();
                    if (buffer[j] == ' ')
                        {
                            buffer[j] = 0;
                            break;
                        }
                    if (buffer[j] == '\n')
                        {
                            buffer[j] = 0;
                            end_of_line = 1;
                            break;
                        }
                }
            /* Store the argument. */
            args[i] = (char *)malloc (strlen (buffer));
            strcpy (args[i], buffer);

            if (end_of_line)
                break;
        }
    /* Check '&' signal. */
    if (strcmp (args[i], "&") == 0)
        return 1;
    else
        return 0;
}

/* This function execute the command args[0] in child process and decide whether the parent process
 * need to wait arroding to flag. */
void exe_command (char *args[], int flag_wait)
{
    pid_t pid = fork ();
    if (pid == -1)
        perror ("failed to frok");
    else if (pid == 0)
        {
            execvp (args[0], args + 1);
            perror ("child: failed to execute");
        }
    else
        {
            if (flag_wait)
                wait (NULL);
        }
}

int main (void)
{
    /* The author puts *args[] in main to adapt to later update for history functionality. */
    char *args[MAX_LINE / 2 + 1] = { NULL };
    char buffer[MAX_LINE];
    int should_run = 1;
    int wait_or_not = -1;

    while (should_run)
        {
            printf ("osh>");
            fflush (stdout);

            /* Read the user inputs. */
            scanf ("%s", buffer);
            if (strcmp (buffer, "exit") == 0)
                should_run = 0;
            else if (strcmp (buffer, "!!") == 0)
                {
                    /* If this is the first command. */
                    if (args[0] == NULL)
                        {
                            perror ("No commands in history");
                            continue;
                        }

                    /* Use the arguments from last command including wait_or_not. */
                    exe_command (args, wait_or_not);
                }
            else
                {
                    /* Store the command name. */
                    args[0] = (char *)malloc (strlen (buffer));
                    strcpy (args[0], buffer);

                    /* Read the following arguments. */
                    wait_or_not = read_command (args);

                    /* Run the child process to execute commands. */
                    exe_command (args, wait_or_not);
                }
            // Then it goes to next round.
        }
    int i;
    for (i = 0; i <= MAX_LINE / 2; ++i)
        free (args[i]);
    return 0;
}
