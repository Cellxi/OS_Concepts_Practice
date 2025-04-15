#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
int main ()
{
    pid_t pid;
    pid = fork ();
    if (pid == -1)
        {
            perror ("fork failed");
            return 1;
        }
    if (pid == 0)
        {
            execlp ("the command not found", "or the arguments wrong",
                    "or it doesn't end with NULL");
            printf ("LINE J\n");
        }
    else
        {
            wait (NULL);
            printf ("Child Complete\n");
        }
    return 0;
}
