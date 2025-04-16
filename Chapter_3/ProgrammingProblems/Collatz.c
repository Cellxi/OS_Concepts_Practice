// ATTENTION!This program is designed for 64-bit arch, assuming long = int64
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define MNAME "CollatzM"
#define CAL_FAILED 0xffffffff   // the error flag of calculate()
#define MAXSZ 512 * 1024 * 1024 // I guess 512MB should be affordable on almost every modern PC
#define INISZ 8192
unsigned long convert (const char *ch) // convert ch[] into integer
{
    unsigned long d = 0, i = 0;
    while (ch[i])
        d *= 10, d += ch[i] - '0', ++i;
    return d;
}
size_t calculate (long *array) // calculate answer within array[]
{
    size_t msize = INISZ;
    array = (long *)realloc (array, msize);
    if (array == NULL)
        fprintf (stderr, "realloc failed (need memory more than %lu)\n", msize);
    int i = 0;
    while (msize <= MAXSZ) // increase the size of array until it hits the limit
        {
            array = realloc (array, msize);
            if (array == NULL)
                fprintf (stderr, "realloc failed (need memory more than %lu)\n", msize);
            while (i < msize / sizeof (long))
                { // illustrate an infinite array within this function
                    if ((array[i] & 1) == 1)
                        array[i + 1] = array[i] * 3 + 1;
                    else
                        array[i + 1] = array[i] >> 1;
                    ++i;
                    if (array[i] == 1)
                        return msize;
                }
            msize <<= 1;
        }
    return CAL_FAILED;
    // the worst situation
}
void child (const int *pp, const int fd,
            const unsigned long target) // child process search the answers
{
    long *routinue = malloc (sizeof (long));
    if (routinue == NULL)
        perror ("malloc failed");
    routinue[0] = target;
    size_t msize = calculate (routinue);
    if (msize == CAL_FAILED)
        perror ("Failed to calculate answer(lack of resources)"), exit (1);

    // we firstly decide the size by dynamic array then send it through pipe to parent
    if (ftruncate (fd, msize) == -1)
        perror ("ftruncate failed"), free (routinue), exit (1);
    long *ptr = (long *)mmap (0, msize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        perror ("mmap failed"), free (routinue), exit (1);
    memcpy (ptr, routinue, msize);                  // copy from child memory to shared
    write (pp[1], (void *)&msize, sizeof (size_t)); // pass msize through pipe
    free (routinue);
    return;
}
void parent (const int *pp, const int fd)
{
    size_t msize;
    read (pp[0], (void *)&msize, sizeof (size_t));
    long *ptr = (long *)mmap (0, msize, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        perror ("parent mmap failed");
    else
        {
            int i;
            for (i = 0; ptr[i] != 1 && i < msize / sizeof (long); ++i)
                if (i % 10 == 9)
                    printf ("%ld\n", ptr[i]);
                else
                    printf ("%ld ", ptr[i]);
            if (ptr[i] == 1)
                printf ("%d\n", 1);
            else
                perror ("parent: failed to revieve data");
            // if child process thought it had found answer but the sequence doesn't end with 1
        }
}
int main (int argc, char *argv[])
{
    if (argc != 2)
        perror ("Usage:Collatz number"), exit (1);
    if (strlen (argv[1]) > 12 || convert (argv[1]) < 0)
        perror ("Too large number"), exit (1);
    int pp[2];
    if (pipe (pp))
        perror ("pipe failed"), exit (1);
    int fd = shm_open (MNAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
        {
            perror ("shm_create failed");
            return 1;
        }
    pid_t pid = fork ();
    if (pid == -1)
        {
            perror ("fork failed");
            return 1;
        }
    if (pid == 0)
        {
            close (pp[0]);
            child (pp, fd, convert (argv[1]));
            close (pp[1]);
        }
    else
        {
            close (pp[1]);
            wait (NULL);
            parent (pp, fd);
            close (pp[0]);
            shm_unlink (MNAME);
        }
    return 0;
}
