#include <stdio.h>
#define MAX_PID 5000
#define MIN_PID 300

/* We regard this as a process table in the simulation. pstat=process state */
char pstat[5120];

/* A PCE represents a process. Its id is pid. */
typedef struct pce
{
    int id;
} PCE;

int allocate_map (void);
int allocate_pid (void);
void release_pid (int pid);
int main () { return 0; }
