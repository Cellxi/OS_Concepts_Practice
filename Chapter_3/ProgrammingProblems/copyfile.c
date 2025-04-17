#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFF_SIZE 128

/* This function copies everything from FROM to TO. */
void copy (int from, int to)
{
    char buffer[BUFF_SIZE];
    ssize_t size_read = read (from, buffer, BUFF_SIZE);
    while (size_read) // read() doesn't return EOF, it returns 0 on EOF.
        {
            if (write (to, buffer, size_read) != size_read)
                perror ("pp: write error"), exit (1);
            size_read = read (from, buffer, BUFF_SIZE);
            if (size_read == -1)
                perror ("Parent: failed to read from source file"), exit (1);
        }
}

int main (int argc, char *argv[])
{
    /* Check input validity. */
    if (argc != 3)
        perror ("Usage:copyfile source target"), exit (1);

    /* We use pp to transfer data and pps to send signals. */
    int pp[2];
    if (pipe (pp))
        perror ("pipe error"), exit (1);

    /* Run the child process. */
    pid_t pid = fork ();
    if (pid == -1)
        perror ("fork error"), exit (1);
    if (pid == 0)
        {
            /* Child process read the content and write it into target file. */
            close (pp[1]);
            int dst = open (argv[2], O_CREAT | O_RDWR, 0644);
            if (dst == -1)
                perror ("dst: failed to open"), exit (1);

            copy (pp[0], dst);
            close (pp[0]);
        }
    else
        {
            /* Parent process open the source file and write the data into pipe. */
            close (pp[0]);
            int src = open (argv[1], O_RDONLY, 0444);
            if (src == -1)
                perror ("src: failed to open"), exit (1);

            copy (src, pp[1]);
            close (pp[1]); // This sends EOF to the pipe.
        }
    return 0;
}
