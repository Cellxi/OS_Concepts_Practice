#include <stdio.h>
#include <time.h>
#include <unistd.h>
int main ()
{
    struct timespec mytime = { 0, 0 };
    clock_gettime (CLOCK_MONOTONIC, &mytime);
    printf ("%ld.%ld\n", mytime.tv_sec, mytime.tv_nsec);
    clock_gettime (CLOCK_MONOTONIC, &mytime);
    printf ("%ld.%ld\n", mytime.tv_sec, mytime.tv_nsec);
    sleep (1);
    clock_gettime (CLOCK_MONOTONIC, &mytime);
    printf ("%ld.%ld\n", mytime.tv_sec, mytime.tv_nsec);
    clock_gettime (CLOCK_MONOTONIC, &mytime);
    printf ("%ld.%ld\n", mytime.tv_sec, mytime.tv_nsec);
    return 0;
}
