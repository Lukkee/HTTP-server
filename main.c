//  Includes
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

//  Parameters
#define PORTNUMBER 8000

//  Main function
int main ( void ) {
    /* Create server socket */
    int server_fd = socket (AF_INET, SOCK_STREAM, 0);

    /* Network configuration */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;          // IPV4
    addr.sin_port = htons (PORTNUMBER);       // PORT NUMBER
    addr.sin_addr.s_addr = INADDR_ANY;  // ANY NETWORKCARD

    /* Attach socket to address */
    bind (server_fd, (struct sockaddr*)&addr, sizeof (addr));
    listen (server_fd, 10);
    printf ("Listening on port %d\n", PORTNUMBER);

    /* Create client socket */
    int client_fd = accept (server_fd, NULL, NULL);

    char buffer[1024];
    recv (client_fd, buffer, sizeof (buffer), 0);
    printf ("Recieved:\n%s\n", buffer);

    close (client_fd);
    close (server_fd);
    return 0;
}