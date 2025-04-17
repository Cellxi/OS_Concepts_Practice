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
            execlp ("/bin/ls", "ls", NULL);
            printf ("LINE J");
        }
    else
        {
            wait (NULL);
            printf ("Child Complete\n");
        }
    return 0;
}
