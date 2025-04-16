#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define MNAME "SHMOFT"
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
    int fd = shm_open (MNAME, O_CREAT | O_RDWR, 0644);
    if (fd == -1)
        {
            perror ("shm_open failed");
            return 1;
        }
    if (ftruncate (fd, SIZE) == -1)
        {
            perror ("ftruncate failed");
            return 1;
        }
    struct timespec *ptr = (struct timespec *)mmap (
        0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        {
            perror ("mmap failed");
            return 1;
        }
    pid_t pid = fork ();
    if (pid == -1)
        {
            perror ("fork failed");
            unlink (MNAME);
            return 1;
        }
    if (pid == 0)
        { // child process
            if (clock_gettime (CLOCK_MONOTONIC, ptr))
                {
                    perror ("gettime failed");
                    return 1;
                }
            execvp (argv[1], argv + 1);
            perror ("execvp failed");
            return 1;
        }
    else
        {
            wait (NULL);
            struct timespec endtime;
            if (clock_gettime (CLOCK_MONOTONIC, &endtime))
                {
                    perror ("gettime failed");
                    return 1;
                }
            time_minus (&endtime, *ptr);
            printf ("%s ended, cost %lu.%09ld seconds\n", argv[1],
                    endtime.tv_sec, endtime.tv_nsec);
        }
    return 0;
}
