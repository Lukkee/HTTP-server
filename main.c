//  Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

//  Parameters
#define PORTNUMBER 8000
#define FILENAME "index.html"

char *generateResponse ( const char* string, const char* content_type) {
    size_t len = strlen (string);
    char *response = malloc(len + 128);
    if (!response) { perror ("memory allocation for response failed"); exit(1); }

    int written = snprintf (
        response, len + 128,
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: %s\r\n"
        "\r\n"  //  end header "\r\n\r\n"
        "%s\r\n",
        len + 2,
        content_type,
        string
    );

    if (written < 0 || (size_t)written >= len + 128) {
        perror ("response formatting failed");
        free (response);
        return NULL;
    }

    return response;
}

char *readFile ( FILE *file ) {
    /* Get filesize */
    fseek (file, 0, SEEK_END);
    long filesize = ftell (file);
    rewind (file);

    /* Allocate memory */
    char *buffer = malloc (filesize + 1);
    if (!buffer) { perror ("memory allocation for file buffer failed"); exit(1); }

    /* Read file into buffer */
    size_t readsize = fread (buffer, 1, filesize, file);
    if (readsize != (size_t)filesize) { 
        perror ("file read failed"); 
        free (buffer);
        exit(1);
    }

    /* Null-terminate and return */
    buffer[filesize] = '\0';
    return buffer;
}


//  Main function
int main ( void ) {
    /* Create server socket */
    int server_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket failed"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Network configuration */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;          // IPV4
    addr.sin_port = htons (PORTNUMBER); // PORT NUMBER
    addr.sin_addr.s_addr = INADDR_ANY;  // ANY NETWORKCARD

    /* Attach socket to address */
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed"); exit(1);
    }
    if (listen(server_fd, 10) < 0) { perror("listen failed"); exit(1); }
    socklen_t len = sizeof(addr);
    getsockname(server_fd, (struct sockaddr*)&addr, &len);
    printf("Listening on port %d\n", ntohs(addr.sin_port));

    /* Open file */
    FILE *file = fopen (FILENAME, "rb");
    if (!file) { perror ("file open failed"); exit(1); };

    char *filecontent = readFile (file);

    /* Create client socket */
    int client_fd = accept (server_fd, NULL, NULL);

    char buffer[1024];
    recv (client_fd, buffer, sizeof (buffer), 0);
    printf ("Recieved:\n%s\n", buffer);

    /* Respond to request */
    char *response = generateResponse(filecontent, "text/html");
    send (client_fd, response, strlen (response), 0);

    /* Free memory */
    free (response);
    free (filecontent);

    /* Close file */
    fclose (file);

    /* Close sockets */
    close (client_fd);
    close (server_fd);
    return 0;
}