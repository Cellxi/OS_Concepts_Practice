#include "uheads.h"
#include <fcntl.h>

/* Backups of std I/O file desciptor. After dup2(), the std fd will remain but points to different
 * file streams. So we need to restore them without STDIN_FILENO and STDOUT_FILENO. */
int infd_backup, outfd_backup;

/* Indicates if the outputs of current command is regarded as the input of next command. */
int flag_pipe;

/* Indicates if the command runs concurrently. */
int flag_wait = 1;

/* Indicates if the command is a repeat. */
int flag_repe;

/* Indicates if the command means quit. */
int flag_exit;

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

/* Read a single word and backup file desciptor. Returns 1 when meeting an end of the line. */
int read_word (char *buffer)
{
    int j;
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
                    buffer[j] = 0;
                    return 1;
                case '<':
                    infd_backup = dup (STDIN_FILENO);
                    redirect (STDIN_FILENO);
                case ' ':
                    buffer[j] = 0;
                    break;
                default:
                    continue;
                }
            if (buffer[j] == 0)
                break;
        }
    return 0;
}

/* This function checks whether the current argument is a special signal ans sets corresponding
 * flags. It will return 1 if buffer isn't a signal. */
int not_flag (char *buffer)
{
    if (strcmp (buffer, "|") == 0)
        flag_pipe = 1;
    else if (strcmp (buffer, "!!") == 0)
        flag_repe = 1;
    else if (strcmp (buffer, "&") == 0)
        flag_wait = 0;
    else if (strcmp (buffer, "exit") == 0)
        flag_exit = 1;
    else
        return 1;
    return 0;
}

/* This function runs again and again in main loop to read user inputs. */
void read_command (char *args[])
{

    /* The flag to indicate the end of a line. */
    int end_of_line = 0;
    char buffer[MAX_LINE];
    int i;

    /* Read until the end of this command. */
    for (i = 0; i <= MAX_LINE / 2 && !end_of_line; ++i)
        {
            /* Read a single word to buffer each time. */
            end_of_line = read_word (buffer);

            /* Check if the word is a flag. */
            if (not_flag (buffer))
                {
                    free (args[i]);
                    args[i] = (char *)malloc (strlen (buffer));
                    strcpy (args[i], buffer);
                }
        }

    /* Leave args untouched. User shouldn't input argument before !! in one line */
    if (flag_repe)
        return;

    /* Set the last argument NULL to avoid misreading the previous. */
    free (args[i]);
    args[i] = NULL;
}
