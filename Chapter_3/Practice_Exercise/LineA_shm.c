#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define MNAME "LinA_shm"
int value = 5;
int SIZE = sizeof (value);

void mmap_error (int fd)
{
    perror ("mmap failed");
    close (fd);
    shm_unlink (MNAME);
    exit (1);
}

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
            int fd;
            fd = shm_open (MNAME, O_CREAT | O_RDWR, 0666);
            if (fd == -1)
                {
                    perror ("shm_create failed");
                    return 1;
                }
            if (ftruncate (fd, SIZE) == -1)
                {
                    perror ("ftruncate failed");
                    close (fd);
                    return 1;
                }
            int *ptr;
            ptr = (int *)mmap (0, sizeof (value), PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED)
                mmap_error (fd);
            *ptr = value + 15;
            return 0;
        }
    else
        {
            wait (NULL);
            int fd;
            fd = shm_open (MNAME, O_RDONLY, 0666);
            if (fd == -1)
                {
                    perror ("shm_open failed");
                    return 1;
                }
            int *ptr;
            ptr = (int *)mmap (0, SIZE, PROT_READ, MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED)
                mmap_error (fd);
            printf ("The value passed across shared memory is %d\n", *ptr);
            shm_unlink (MNAME);
            return 0;
        }
}
