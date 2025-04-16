#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
const size_t SIZE = sizeof (struct timespec);

void time_minus (struct timespec *a,
                 const struct timespec b) // this function calculates a minus b
{
    a->tv_nsec -= b.tv_nsec;
    if (a->tv_nsec < 0)
        {
            a->tv_nsec += 1000000000;
            a->tv_sec--;
        }
    a->tv_sec -= b.tv_sec;
}

int main (int argc, char *argv[])
{
    if (argc <= 1)
        {
            perror ("Usage:./tcommand command [arguments...]");
            return 1;
        }
    int fd[2];
    if (pipe (fd))
        {
            perror ("pipe failed");
            return 1;
        }
    pid_t pid = fork ();
    if (pid == -1)
        {
            perror ("fork failed");
            return 1;
        }
    if (pid == 0)
        { // child process
            close (fd[0]);
            struct timespec start;
            if (clock_gettime (CLOCK_MONOTONIC, &start))
                {
                    perror ("gettime failed");
                    return 1;
                }
            if (write (fd[1], (void *)&start, SIZE) == -1)
                {
                    perror ("write error");
                    return 1;
                }
            close (fd[1]);
            execvp (argv[1], argv + 1);
            perror ("execvp failed");
            return 1;
        }
    else
        {
            wait (NULL);
            close (fd[1]);
            struct timespec endtime, start;
            if (clock_gettime (CLOCK_MONOTONIC, &endtime))
                {
                    perror ("gettime failed");
                    return 1;
                }
            if (read (fd[0], (void *)&start, SIZE) == -1)
                {
                    perror ("read error");
                    return 1;
                }
            close (fd[0]);
            time_minus (&endtime, start);
            printf ("%s ended, costing %lu.%09ld seconds\n", argv[1],
                    endtime.tv_sec, endtime.tv_nsec);
        }
    return 0;
}
