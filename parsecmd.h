#include <unistd.h>
#ifndef __PARSECMD_H__
#define __PARSECMD_H__

// Parse functions
int lookup(char *, const char **, int); 
int lookupCmd(char *);
int parseCommand(char *, Command *);
#endif