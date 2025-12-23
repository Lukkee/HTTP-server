//  Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

//  Parameters
#define PORTNUMBER 8000

char *generateResponse ( const char* string ) {
    size_t len = strlen (string);
    char *response = malloc(len + 128);
    if (!response) { perror ("memory allocation for response failed"); exit(1); }

    int written = snprintf (
        response, len + 128,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"  //  end header "\r\n\r\n"
        "%s\r\n",
        len + 2,
        string
    );

    if (written < 0 || (size_t)written >= len + 128) {
        perror ("response formatting failed");
        free (response);
        return NULL;
    }

    return response;
}

//  Main function
int main ( void ) {
    /* Create server socket */
    int server_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) perror("server socket failed");

    /* Network configuration */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;          // IPV4
    addr.sin_port = htons (PORTNUMBER); // PORT NUMBER
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

    /* Respond to request */
    char *response = generateResponse ("Hello!");
    send (client_fd, response, strlen (response), 0);

    /* Free memory */
    free (response);

    /* Close sockets */
    close (client_fd);
    close (server_fd);
    return 0;
}