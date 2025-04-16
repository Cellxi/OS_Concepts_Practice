/* ATTENTION! This program is designed for 64-bit arch, assuming long = int64.
 * It reads a number from shell and outputs the sequence of this number becoming 1 in Collatz's
 * rules. It implements an IPC between parent and child processes with both shared memory and
 * pipe. The child process copies the file desciptor of their shared memory object and truncates it
 * to the desired size, then pass the size to parent process through pipe. Finally the parent
 * process receives the sequence and outputs it. */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define MNAME "CollatzM" // The name of shared memory object file, which will be created in /dev/shm
#define CAL_FAILED -1    // The error flag of calculate().-1 is treated as 2^64-1 in size_t.
#define MAXSZ 512 * 1024 * 1024 // I guess 512MB should be affordable on almost every modern PC
#define INISZ 8192              // Initiate size of the array used to verify the number.

/* Convert ch[] into integer*/
unsigned long convert (const char *ch)
{
    unsigned long d = 0, i = 0;
    while (ch[i])
        d *= 10, d += ch[i] - '0', ++i;
    return d;
}

/* Verifies the number stored at array[0] and records its trace of becoming 1.*/
size_t calculate (long *array)
{
    size_t msize = INISZ;
    int i = 0;

    /* We increase the size of array until it hits the limit. */
    while (msize <= MAXSZ)
        {
            array = realloc (array, msize);
            if (array == NULL)
                fprintf (stderr, "realloc failed (need memory more than %lu)\n", msize);

            /* This loop only runs when there is enough space of array, illustrateing an infinite
             * array where we can focuse on calculating.*/
            while (i < msize / sizeof (long))
                {
                    if ((array[i] & 1) == 1)
                        array[i + 1] = array[i] * 3 + 1;
                    else
                        array[i + 1] = array[i] >> 1;
                    ++i;
                    if (array[i] == 1)
                        return msize;
                }
            msize <<= 1; // Double the expected size of array to adjust.
        }

    /* Return the macro when we run out of memory. */
    return CAL_FAILED;
}
void child (const int *pp, const int fd,
            const unsigned long target) // child process search the answers
{
    /* Create the array used for calculating. */
    long *routinue = malloc (sizeof (long));
    if (routinue == NULL)
        perror ("malloc failed");
    routinue[0] = target;

    /* Try to get answer in routine[] and size of it at msize. */
    size_t msize = calculate (routinue);
    if (msize == CAL_FAILED)
        perror ("Failed to calculate answer(lack of resources)"), exit (1);

    /* We truncate the shared memory object to proper size and map it to the virtual memory of
     * child process. */
    if (ftruncate (fd, msize) == -1)
        perror ("ftruncate failed"), free (routinue), exit (1);
    long *ptr = (long *)mmap (0, msize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        perror ("mmap failed"), free (routinue), exit (1);

    /* Copy data from child memory to shared one. And pass msize through pipe. */
    memcpy (ptr, routinue, msize);
    write (pp[1], (void *)&msize, sizeof (size_t));
    free (routinue);
    return;
}
void parent (const int *pp, const int fd)
{
    /* Recieve the size of shared memory,all of which should be filled with long integers. */
    size_t msize;
    read (pp[0], (void *)&msize, sizeof (size_t));

    /* Map the MNAME object to parent memory. */
    long *ptr = (long *)mmap (0, msize, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
        perror ("parent mmap failed");
    else
        {
            /* Print answer sequence. */
            int i;
            for (i = 0; ptr[i] != 1 && i < msize / sizeof (long); ++i)
                if (i % 10 == 9)
                    printf ("%ld\n", ptr[i]);
                else
                    printf ("%ld ", ptr[i]);

            /* Check the ending. */
            if (ptr[i] == 1)
                printf ("%d\n", 1);
            else
                perror ("parent: failed to revieve data");
            /* Failing to transfer data correctly is the most possible reason I can imagine if child
             * process thought it had found answer but the sequence actually doesn't end with 1.*/
        }
}
int main (int argc, char *argv[])
{
    /* Check if we can handle the input. */
    if (argc != 2)
        perror ("Usage:Collatz number"), exit (1);
    if (strlen (argv[1]) > 12 || convert (argv[1]) < 0)
        perror ("Too large number"), exit (1);

    /* Set pipes and create shared memory object file. */
    int pp[2];
    if (pipe (pp))
        perror ("pipe failed"), exit (1);
    int fd = shm_open (MNAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
        {
            perror ("shm_create failed");
            return 1;
        }

    /* Run child process */
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
        { // parent process
            close (pp[1]);
            wait (NULL);
            parent (pp, fd);
            close (pp[0]);
            shm_unlink (MNAME);
        }
    return 0;
}
