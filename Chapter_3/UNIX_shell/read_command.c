#include "uheads.h"
#include <fcntl.h>

/* Backups of std I/O file desciptor. After dup2(), the std fd will remain but points to different
 * file streams. So we need to restore them without STDIN_FILENO and STDOUT_FILENO. */
int infd_backup, outfd_backup;

/* Indicates if the outputs of current command is regarded as the input of next command. */
int flag_pipe;

/* This function read the file name to redirect std. */
void redirect (int from)
{
    int i;
    char name[MAX_LINE];
    for (i = 0; i < MAX_LINE; ++i)
        {
            name[i] = getchar ();
            if (name[i] == '\n')
                break;
        }
    name[i] = 0;

    /* to means where the std I/O will be direct to. */
    int to = open (name, O_RDWR, 0666);
    if (to == -1)
        {
            perror ("fail to open");
            return; // This will finally return to main loop.
        }
    if (dup2 (to, from) == -1)
        {
            perror ("dup2() failed");
            return; // And continue the command without redirect
        }
    close (to); // Break the link of fd to its file.
}

/* This function runs again and again in main loop to read user inputs. */
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
                        case '>':
                            outfd_backup = dup (STDOUT_FILENO);
                            redirect (STDOUT_FILENO);
                        case EOF:
                        case '\n':
                            end_of_line = 1; // It goes to next case then.
                        case ' ':
                            buffer[j] = 0;
                            break;
                        case '<':
                            infd_backup = dup (STDIN_FILENO);
                            redirect (STDIN_FILENO);
                            buffer[j] = 0;
                            break;
                        case '!':
                            if (j > 0 && buffer[j - 1] == '!')
                                {
                                    while (buffer[j] != '\n')
                                        buffer[j] = getchar ();
                                    int k;
                                    for (k = 0; args[k + 1] != NULL; ++k)
                                        printf ("%s ", args[k]);
                                    printf ("%s\n", args[k]);
                                    return flag_wait;
                                } // Initiation doesn't matter.
                        default:
                            continue;
                        }
                    if (buffer[j] == 0)
                        break;
                }
            /* Store the argument. */
            free (args[i]);
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
