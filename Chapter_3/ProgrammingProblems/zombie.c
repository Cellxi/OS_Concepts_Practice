#include <stdio.h>
#include <unistd.h>
int main ()
{
    pid_t pid = fork ();
    if (pid == -1)
        {
            perror ("");
            return 1;
        }
    if (pid == 0)
        { // child process
            fprintf (stdout, "child process is exiting\n");
            return 0;
        }
    else
        { // parent process
            sleep (1);
            printf ("sleeping.");
            fflush (stdout);
            sleep (10);
            printf (".");
            fflush (stdout);
            sleep (10);
            printf (".");
            fflush (stdout);
            sleep (10);
            fprintf (stdout, "parent wakes up\n");
        }
    return 0;
}
