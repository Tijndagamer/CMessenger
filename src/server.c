// Made by Tijndagamer
// Released under the MIT license

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void server(int port);

// Display the error & exit
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    pthread_t server_thread1, server_thread2;
    const int port1 = 5005;
    const int port2 = 5006;
    int result_server_thread1, result_server_thread2;

    result_server_thread1 = pthread_create(&server_thread1, NULL, server, (void*) port1);
    result_server_thread2 = pthread_create(&server_thread2, NULL, server, (void*) port2);

    pthread_join(server_thread1, NULL);
    pthread_join(server_thread2, NULL);

    return 0;
}

void server(int port)
{
    int sockfd, newsockfd;
    int client_length;
    int n;
    char buffer[256];
    struct sockaddr_in server_addr, client_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    
    // Convert port to network byte order and assign the port of the server
    server_addr.sin_port = htons(port);
    
    // Set the server address to the IP of the host
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        error("ERROR binding");
    }

    listen(sockfd, 5);

    while (1)
    {
        bzero(buffer, 256);
        client_length = sizeof(client_addr);
        char client_nickname[256];

        newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_length);
        if (newsockfd < 0) { error("ERROR accepting"); }

        // Get the nickname of the client
        n = read(newsockfd, client_nickname, 255);
        if (n < 0) { error("ERROR reading from socket while getting nickname"); }
        strtok(client_nickname, "\n");

        printf("Connection established with %s on port %d\n", client_nickname, port);

        bzero(buffer, 256);

        // Read the socket
        while (1)
        {
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 255);
            if (n < 0) { error("ERROR reading from socket"); }
            printf("<%s> %s", client_nickname, buffer);
            if (strcmp(buffer,"--EXIT--\n") == 0) { break; }
        }

        printf("Connection with %s on port %d closed.\n", client_nickname, port);
    }
}
