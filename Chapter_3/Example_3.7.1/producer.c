#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
int main (void)
{
    const int SIZE = 4096;
    const char *name = "OS_Example";
    const char *message_0 = "Hello ";
    const char *message_1 = "World!\n";
    int fd;
    char *ptr;
    fd = shm_open (name, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
        perror ("shm_open()");
    if (ftruncate (fd, SIZE) == -1)
        perror ("ftruncate()");
    ptr = (char *)mmap (0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        perror ("mmap()");
    sprintf (ptr, "%s", message_0);
    ptr += strlen (message_0);
    sprintf (ptr, "%s", message_1);
    ptr += strlen (message_1);
    return 0;
}
