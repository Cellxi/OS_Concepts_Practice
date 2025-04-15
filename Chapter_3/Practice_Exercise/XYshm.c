#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define MNAME "XYshm"
#define NUM_COUNT 5
const size_t SIZE = NUM_COUNT * sizeof (int);
int main ()
{
    int fd;
    fd = shm_open (MNAME, O_CREAT | O_RDWR, 0644);
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
    int *ptr;
    ptr = (int *)mmap (0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        {
            perror ("mmap failed");
            return 1;
        }
    int i;
    for (i = 0; i < NUM_COUNT; ++i)
        ptr[i] = i;
    pid_t pid = fork ();
    switch (pid)
        {
        case -1:
            perror ("fork failed");
            return 1;
        case 0:
            {
                int i;
                for (i = 0; i < 5; ++i)
                    {
                        ptr[i] *= -i;
                        printf ("Child:%d\n", ptr[i]);
                    }
                break;
            }
        default:
            {
                wait (NULL);
                int i;
                for (i = 0; i < 5; ++i)
                    printf ("Parent:%d\n", ptr[i]);
            }
        }
    return 0;
}
