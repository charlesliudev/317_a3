#include <unistd.h>
#ifndef _SERVERH__

#define _SERVERH__

// Server functions 
void initServer(int port);
int createSocket(int port);
void genPort(Port *);
void getIP(int, int *);
void writeState(State *);
int acceptConnection(int);
void handleZombie(int);
#endif