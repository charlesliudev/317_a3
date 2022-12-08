#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>
#include <netinet/in.h>
#include <limits.h>
#include <dirent.h>
#include "definitions.h"

/**
 *  Creates socket and specified port and starts listening to this socket
 * @param port, the port to listen on
 * Return a File Descriptor of the socket
 * */

int createSocket(int port)
{
    // Server address
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = { INADDR_ANY },
    };

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Cannot open socket");
        exit(EXIT_FAILURE);
    }

    // Enable address reuse
    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Bind socket to port
    if (bind(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Can't bind to address");
        exit(EXIT_FAILURE);
    }

    printf("Listening for requests on port %d \n", port);
    listen(sock, 5);
    return sock;
}

/**
 * Generate two random numbers to be used for the port in passive mode
 * @param port: Port struct pointer to be used
 */
void genPort(Port* port)
{
    srand(time(NULL));
    port->p1 = 120 + (rand() % 64);
    port->p2 = rand() % 301;
}

void handleZombie(int signum)
{
    int status;
    printf("Handle zombie: %d %ld", status, &status);
    wait(&status);
}

void getIP(int sock, int* ip)
{
    socklen_t addr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in addr;
    char host[INET_ADDRSTRLEN];
    getsockname(sock, (struct sockaddr*)&addr, &addr_size);
    inet_ntop(AF_INET, &(addr.sin_addr), host, INET_ADDRSTRLEN);
    sscanf(host, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
}

int acceptConnection(int socket)
{
    int addressLength = 0;
    struct sockaddr_in client_address;
    addressLength = sizeof(client_address);
    return accept(socket, (struct sockaddr*)&client_address, &addressLength);
}