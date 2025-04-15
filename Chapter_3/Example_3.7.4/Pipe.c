#include <stdio.h>
#include <unistd.h>

// READ_END=0 WRITE_END=1
#define BUFF_SIZE 25

int main (void)
{
    char write_msg[BUFF_SIZE] = "Greetings\n";
    char read_msg[BUFF_SIZE];
    int fd[2];
    pid_t pid;

    if (pipe (fd) == -1)
        {
            perror ("Pipe failde");
            return 1;
        }
    pid = fork ();
    if (pid == -1)
        {
            perror ("Fork failed");
            return 1;
        }

    if (pid == 0)
        {
            close (fd[1]);
            read (fd[0], read_msg, BUFF_SIZE);
            printf ("child process read:%s", read_msg);
            close (fd[0]);
        }
    else
        {
            close (fd[0]);
            write (fd[1], write_msg, BUFF_SIZE);
            close (fd[1]);
        }
    return 0;
}
