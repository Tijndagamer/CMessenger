#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, port, n;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];

    // Check arguments
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(0);
    }

    // Same as in server.c
    port = atoi(argv[2]);
//    port = 5005;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    // Speaks for itself
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
    }

    // Same as in server.c
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // Mostly same as in server.c
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    // Connect to the server
    if (connect(sockfd, &server_addr, sizeof(server_addr)) < 0)
    {
        error("ERROR connecting");
    }

    while (1)
    {

        // Send message
        bzero(buffer, 256);
        printf("<you> ");
        fgets(buffer, 255, stdin);
//        printf("%s", buffer);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) { error("ERROR writing to socket"); }

        // Receive message
//        bzero(buffer, 256);
//        n = read(sockfd, buffer, 255);
//        if (n < 0) { error("ERROR reading from socket"); }
//        printf("%s\n", buffer);
    }

    return 0;
}
