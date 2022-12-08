#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <limits.h>
#include <dirent.h>
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

typedef enum conn_mode
{
  NORMAL, // normal
  SERVER // pasv server listens
} conn_mode;

typedef enum cmdlist
{
  CWD,
  NLST,
  PASV,
  QUIT,
  RETR,
  TYPE,
  USER,
  CDUP,
  STRU,
  MODE,
} cmdlist;

static const char *cmdlist_str[] =
    {
        "CWD", "NLST", "PASV", "QUIT", "RETR",
        "TYPE", "USER", "CDUP", "STRU", "MODE"};

static const char *usernames[] =
    {
        "cs317",
};

typedef struct Command
{
  char command[5]; 
  char arg[BUFFER_SIZE]; 
  int cmdIndex; 
} Command;

typedef struct Port
{
  int p1; 
  int p2; 
} Port;

typedef struct State
{
  int mode; // normal, server
  int logged_in;
  char *username;
  char *message;
  int connection;
  int sock_pasv;
} State;


static char *welcome_message = "Hello! You are connected.";
char start_dir[PATH_MAX]; 