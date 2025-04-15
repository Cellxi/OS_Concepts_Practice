#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#define SIZE 5
int num[SIZE] = { 0, 1, 2, 3, 4 };

int main ()
{
    pid_t pid = fork ();
    switch (pid)
        {
        case -1:
            perror ("");
            return 1;
        case 0:
            {
                int i;
                for (i = 0; i < 5; ++i)
                    {
                        num[i] *= -i;
                        printf ("Child:%d\n", num[i]);
                    }
                break;
            }
        default:
            {
                wait (NULL);
                int i;
                for (i = 0; i < 5; ++i)
                    printf ("Parent:%d\n", num[i]);
            }
        }
    return 0;
}
