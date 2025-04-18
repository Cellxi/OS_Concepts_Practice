#ifndef MAX_LINE

#include <ctype.h> // It includes type judge functions.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80

extern int outfd_backup, infd_backup;

/* Input handle in main loop. */
int read_command (char *args[]);

#endif
