#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
int value = 5;

int main (void)
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
            value += 15;
            return 0;
        }
    else
        {
            wait (NULL);
            printf ("Value in parent:%d\n", value);
            return 0;
        }
}
