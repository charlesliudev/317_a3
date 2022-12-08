#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include "definitions.h"

/**
 * Lookup the needle (command) within the haystack (cmdlist_str)
 * @param needle: char pointer (E.g the raw command sent from the server cmd->arg)
 * @param haystack: char array of pointers to search for needle (E.g cmdlist)
 * @param count, the length of the haystack (supported commands by server)
 * @return the index where the command is found in the haystack, else -1 
 */

int lookup(char* needle, const char** haystack, int count)
{
    int i;
    char* cmd;
    if ((cmd = strtok(needle, "\r\n")) != NULL) {
        for (i = 0; i < count; i++) {
            int result;
            result = strcasecmp(cmd, haystack[i]);
            if (!result) {
                return i; 
            }
        }
        return -1; 
    }
    else {
        return -1; 
    }
}

int lookupCmd(char* cmd)
{
    const int cmdlist_count = sizeof(cmdlist_str) / sizeof(char*);
    return lookup(cmd, cmdlist_str, cmdlist_count);
}

int parseCommand(char* cmdstring, Command* cmd)
{
    if (cmdstring == NULL) {
        printf("Server info: Error reading cmdstring\n");
        return -1; 
    }
    sscanf(cmdstring, "%s %s", cmd->command, cmd->arg);
    char* clean_cmd = strtok(cmd->command, "\r\n"); 
    if (clean_cmd != NULL) {
        strcpy(cmd->command, clean_cmd);
        printf("Server info: Clean cmd:%s\n", cmd->command);
    }
    char* clean_arg = strtok(cmd->arg, "\r\n");
    if (clean_arg != NULL) {
        strcpy(cmd->arg, clean_arg);
        printf("Server info: Clean arg: %s\n", clean_arg);
    }
 
    int cmdListIndex = lookupCmd(cmd->command); // Lookup the clean command
    if (cmdListIndex != -1) {
        strcpy(cmd->command, cmdlist_str[cmdListIndex]);
        cmd->cmdIndex = cmdListIndex;
    }
    else {
        printf("Server info: Unable to parse unknown command %s \n", cmd->command);
        return -1; // Unable to parse Command from user input
    }
}