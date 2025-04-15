#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

int main (void)
{
    const int SIZE = 4096;
    const char *name = "OS_Example";
    int fd;
    char *ptr;
    fd = shm_open (name, O_RDONLY, 0666);
    if (fd == -1)
        perror ("shm_open()");
    ptr = (char *)mmap (0, SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        perror ("mmap()");
    printf ("%s", ptr);
    shm_unlink (name);
    return 0;
}
