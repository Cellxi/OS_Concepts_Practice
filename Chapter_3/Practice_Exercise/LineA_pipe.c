#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 16
int value = 5;

int main (void)
{
    int fd[2];
    pid_t pid;

    if (pipe (fd) == -1)
        {
            perror ("pipe failed");
            return 1;
        }

    pid = fork ();
    if (pid == -1)
        {
            perror ("fork failed");
            return 1;
        }
    if (pid == 0)
        {
            value += 15;
            close (fd[0]);
            write (fd[1], &value, sizeof (value));
            close (fd[1]);
            return 0;
        }
    else
        {
            wait (NULL);
            printf ("Value in parent:%d\n", value);
            close (fd[1]);
            read (fd[0], &value, sizeof (value));
            close (fd[0]);
            printf ("Value from child:%d\n", value);
            return 0;
        }
}
