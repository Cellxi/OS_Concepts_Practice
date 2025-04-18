#ifndef MAX_LINE

#include <ctype.h> // It includes type judge functions.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 80

extern int outfd_backup, infd_backup;
extern int flag_pipe;
extern int flag_wait;
extern int flag_repe;
extern int flag_exit;

/* Input handle in main loop. */
void read_command (char *args[]);

#endif
