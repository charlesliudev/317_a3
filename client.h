#include <unistd.h>
#ifndef _HANDLESH__

#define _HANDLESH__

// Command Handler functions
/**
 * ftp<COMMAND> functions have similar parameters and have 
 * Functionality based on their name (E.g ftpQuit deals with QUIT command)
 * @param Command *, Command struct of what Client has sent
 * @param State *, State of the current client connection
 */

void response(Command *, State *);
void ftpCwd(Command *, State *);
void ftpCdup(State *);
void ftpType(Command *, State *);
void ftpStru(Command *, State *);
void ftpMode(Command *, State *);
void ftpPasv(Command *, State *);
void ftpNlst(Command *, State *);
void ftpRetr(Command *, State *);
void ftpUser(Command *, State *);
void ftpQuit(State *);
#endif
