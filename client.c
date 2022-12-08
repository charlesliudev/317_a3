#include "helper.h"
#include "server.h"
#include "handles.h"
#include "parsecmd.h"
#include "dir.h"
#include <strings.h>

#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>

/**
 * Handles client request and generates response message for client
 * @param Command: Current cmd from client
 * @param State: Current connection state from client
 */

void response(Command* cmd, State* state)
{
    printf("Command: %s\n", cmd->command);

    if (cmd->cmdIndex == USER) {
        ftpUser(cmd, state);
    } else if (cmd->cmdIndex == PASV) {
        ftpPasv(cmd, state);
    } else if (cmd->cmdIndex == CWD) {
        ftpCwd(cmd, state);
    } else if (cmd->cmdIndex == CDUP) {
        ftpCdup(state);
    } else if (cmd->cmdIndex == NLST) {
        ftpNlst(cmd, state);
    } else if (cmd->cmdIndex == STRU) {
        ftpStru(cmd, state);
    } else if (cmd->cmdIndex == MODE) {
        ftpMode(cmd, state);
    } else if (cmd->cmdIndex == RETR) {
        ftpRetr(cmd, state);
    } else if (cmd->cmdIndex == QUIT) {
        ftpQuit(state);
    } else if (cmd->cmdIndex == TYPE) {
        ftpType(cmd, state);
    } else {
        state->message = "500: Bad command.\n";
        writeState(state);
    }
}

int validateFilePath(char* arg)
{
    if (!arg || !arg[0]) {
        printf("No command\n");
        return -6;
    }

    if (strstr(arg, "../") != NULL) {
        printf("Command contains ../\n");
        return -1;
    }

    if (strstr(arg, "./") == &arg[0]) {
        printf("Command begins with ./\n");
        return -2;
    }

    if (strcmp(arg, "..") == 0) {
        printf("Command is ..\n");
        return -3;
    }

    if (strcmp(arg, ".") == 0) {
        printf("Command is .\n");
        return -4;
    }

    if (strstr(arg, "/") == &arg[0]) {
        printf("Command is an abosulte path\n");
        printf("Starting dir is %s\n", start_dir);

        if (strcmp(start_dir, arg) == 0) {
            return 1;
        }

        char start_dir_buff[256];
        memset(start_dir_buff, 0, 256);
        strcpy(start_dir_buff, start_dir);
        strcat(start_dir_buff, "/"); 

        if (strstr(arg, start_dir_buff) == &arg[0]) {
            printf("Valid: Absolute path starting with starting directory \n");
            return 1;
        } else {
            printf("Invalid: Absolute path that doesn't start with starting directory \n");
            return -5;
        }
    }

    printf("Valid Command\n");
    return 1;
}


void ftpUser(Command* cmd, State* state)
{
    if (state->logged_in) {
        state->message = "530: Already logged in \n";
    } else {
        int isValidUsername = 0;
        for (int i = 0; i < 20; i++) {
            if (strcasecmp(cmd->arg, usernames[i]) == 0) {
                isValidUsername = 1;
                break;
            }
        }

        if (isValidUsername) {
            state->username = malloc(40);
            memset(state->username, 0, 40);
            strcpy(state->username, cmd->arg);
            state->logged_in = 1;
            state->message = "230: Login successful.\n";
        } else {
            state->message = "530: Invalid username.\n";
        }
    }
    writeState(state);
}


void ftpQuit(State* state)
{
    state->message = "Goodbye.\n";
    writeState(state);
    close(state->connection);
    printf("Disconnected.\n");
    exit(0);
}

void ftpCwd(Command* cmd, State* state)
{
    if (state->logged_in) {
        int checkresponse = validateFilePath
    (cmd->arg); 

        if (checkresponse == -1) {
            state->message = "550: Directory path cannot contain ../ \n";
        }
        else if (checkresponse == -2) {
            state->message = "550: Directory path cannot begin with ./ \n";
        }
        else if (checkresponse == -3) {
            state->message = "550: Directory Path cannot be ..\n";
        }
        else if (checkresponse == -4) {
            state->message = "550: Directory Path cannot be .\n";
        }
        else if (checkresponse == -5) {
            state->message = "550: Directory path must start with the starting directory.\n";
        }
        else if (checkresponse == -6) {
            state->message = "501: Syntax error. \n";
        }
        else if (checkresponse == 1) {
            if (chdir(cmd->arg) == 0) {
                state->message = "250: Directory changed.\n";
            }
            else {
                state->message = "550: Failed to change.\n";
            }
        }
        else {
            state->message = "550: Failed to change.\n";
        }
    }
    else {
        state->message = "503: Please login.\n";
    }
    writeState(state);
}

void ftpStru(Command* cmd, State* state)
{
    if (state->logged_in) {
        if (strcasecmp(cmd->arg, "F") == 0) {
            state->message = "200: Structure set to file.\n";
        } else if (strcasecmp(cmd->arg, "P") == 0) {
            state->message = "504: Page is not supported.\n";
        } else if (strcasecmp(cmd->arg, "R") == 0) {
            state->message = "504: Record not supported.\n";
        } else {
            state->message = "501: Bad argument.\n";
        }
    } else {
        state->message = "530: Please login.\n";
    }
    writeState(state);
}


void ftpCdup(State* state)
{
    if (!state->logged_in) {
        state->message = "530: Please login.\n";
        return;
    }

    char cwd[PATH_MAX];
    memset(cwd, 0, sizeof(cwd));

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        state->message = "550: Action could not be taken. \n";
        return;
    }

    if (strcasecmp(start_dir, cwd) == 0) {
        state->message = "550: Action not allowed. \n";
    }
    else {
        if (chdir("..") == 0) {
            state->message = "200: Directory changed to parent.\n";
        }
        else {
            state->message = "550: Action could not be taken. \n";
        }
    }

    memset(cwd, 0, sizeof(cwd));
    writeState(state);
}


void ftpNlst(Command* cmd, State* state)
{
    char curDir[BUFFER_SIZE];
    memset(curDir, 0, BUFFER_SIZE);

    if (!state->logged_in) {
        state->message = "503: Login please.\n";
        writeState(state);
        return;
    }

    if (state->mode != SERVER) {
        state->message = "425: Use PASV first to open data connction.\n";
        writeState(state);
        return;
    }

    if (cmd->arg == NULL || cmd->arg[0] == '\0') {
        // must have no argument supplied
        int connection; // pasv connection
        connection = acceptConnection(state->sock_pasv);
        printf("Opened pasv connection.\n");
        state->message = "150: Valid file status. Directory listing.\n";
        writeState(state);

        getcwd(curDir, BUFFER_SIZE);
        listFiles(connection, curDir);
        close(connection);
        close(state->sock_pasv);
        printf("Files sent successfully to pasv connection. Closed connection.\n");

        state->mode = NORMAL; 

        printf("Mode is normal again.\n");

        state->message = "226: Closing connection. Sent direcotory.\n";
    }
    else {
        state->message = "501: Invalid action.\n";
        writeState(state);
        return;
    }
}

void ftpMode(Command* cmd, State* state)
{
    printf("Server info: MODE command\n");
    if (state->logged_in) {
        if (strcasecmp(cmd->arg, "S") == 0) {
            state->message = "200: Mode is S. Valid.\n";
        }
        else if (strcasecmp(cmd->arg, "C") == 0) {
            state->message = "504: MODE Compressed not supported.\n";
        }
        else if (strcasecmp(cmd->arg, "B") == 0) {
            state->message = "504: MODE Block not supported.\n";
        }
        else {
            state->message = "501: Bad.\n";
        }
    }
    else {
        state->message = "530: Login please.\n";
    }
    writeState(state);
}


void ftpPasv(Command* cmd, State* state)
{
    int ip[4] = { 0 }; // ipv4 address
    char message_buf[256] = { 0 };
    Port* port = malloc(sizeof(Port));

    printf("Server info: PASV command\n");

    if (state->logged_in) {
        char* response = "227: Passive Mode (%d,%d,%d,%d,%d,%d)\n";
        genPort(port);
        getIP(state->connection, ip);

        if (state->sock_pasv) {
            close(state->sock_pasv);
        }

        printf("Random port 1: %d\n", port->p1);
        printf("Random port 2: %d\n", port->p2);
        state->sock_pasv = createSocket((256 * port->p1) + port->p2);
        printf("Listening on PASV port: %d\n", 256 * port->p1 + port->p2);
        sprintf(message_buf, response, ip[0], ip[1], ip[2], ip[3], port->p1, port->p2);
        state->message = message_buf;
        state->mode = SERVER; 
        printf("Status: %s\n", state->message);
    } else {
        state->message = "530: Login please.\n";
    }

    writeState(state);
    free(port);
}


void ftpType(Command* cmd, State* state)
{
    if (!state->logged_in) {
        state->message = "530: Login please.\n";
        writeState(state);
        return;
    }

    if (strcasecmp(cmd->arg, "I") == 0) {
        state->message = "200: Binary mode.\n";
    }
    else if (strcasecmp(cmd->arg, "A") == 0) {
        state->message = "200: ASCII mode.\n";
    }
    else if (cmd->arg != NULL && (cmd->arg[0] == '\0')) {
        state->message = "501: Syntax Error.\n";
    }
    else {
        state->message = "504: Error.\n";
    }

    writeState(state);
}

void ftpRetr(Command* cmd, State* state)
{
    if (!state->logged_in) {
        state->message = "530: Login please.\n";
        writeState(state);
        return;
    }

    if (state->mode != SERVER) {
        state->message = "425: Use pasv to open data connection.\n";
        writeState(state);
        return;
    }

    int response = validateFilePath
(cmd->arg);
    if (response < 0) {
        state->message = "550: File path not reachable.\n";
        writeState(state);
        return;
    }

    int connection;
    int fd; // file descriptor from opening the file
    struct stat stat_buf;
    int sent_total = 0;
    off_t offset = 0;
    fd = open(cmd->arg, O_RDONLY);
    // Is file readable and open the file for read-only
    if (access(cmd->arg, R_OK) == 0 && fd != -1) {
        fstat(fd, &stat_buf); // get file stats
        state->message = "150: File status bad. Open binary mode.\n";
        writeState(state);

        connection = acceptConnection(state->sock_pasv);
        if (sent_total = sendfile(connection, fd, &offset, stat_buf.st_size)) {

            if (sent_total != stat_buf.st_size) {
                perror("Not done transfer.");
                exit(EXIT_SUCCESS);
            }

            state->message = "226: Good file send closing connection. \n";
        }
        else {
            state->message = "550: Can't read file.\n";
        }
        close(fd);
        close(connection);
    }
    else {
        state->message = "550: Can't get file. \n";
    }
    close(state->sock_pasv);
    state->mode = NORMAL;

    writeState(state);
}
