// Made by Tijndagamer
// Released under the MIT license

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

void sender(char *server_char);
void receiver(int needs_arg);

void error(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int need_arg = 5;
    pthread_t sender_thread, receiver_thread;
    int sender_thread_result, receiver_thread_result;

    sender_thread_result = pthread_create(&sender_thread, NULL, sender, (void*) argv[1]);
    receiver_thread_result = pthread_create(&receiver_thread, NULL, receiver, (void*) need_arg);

    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);

    return 0;
}

void sender(char *server_char)
{
    const int port = 5005;
    int attempts = 50; /* Maximum amount of connection attempts */
    int sockfd, n, connect_result;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];

    // Same as in server.c
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { error("ERROR opening socket"); }

    // Speaks for itself
    server = gethostbyname(server_char);
    if (server == NULL) { error("ERROR, no such host\n"); }

    // Same as in server.c
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // Mostly same as in server.c
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    // Connect to the server
    while (attempts > 0)
    {
        connect_result = connect(sockfd, &server_addr, sizeof(server_addr));
        if(connect_result < 0)
        {
            // Connection has failed, try again
            printf("Connection error, trying again in 1 second...\n");
            sleep(1);
            attempts--;
        } else {
            // Connection worked, we can stop the loop
            printf("Got a connection!\n");
            break;
        }
    }
    if (attempts < 1) { error("Connecting failed to much.\n"); }

    // Get & send the nickname
    char nickname[256];
    bzero(nickname, 256);
    printf("Nickname = ");
    fgets(nickname, 255, stdin);
    n = write(sockfd, nickname, strlen(nickname));

    while (1)
    {
        // Send message
        bzero(buffer, 256);
        printf("<you> ");
        fgets(buffer, 255, stdin);

        // Check for exit command
        if (strcmp(buffer,"--EXIT--\n") == 0)
        {
            n = write(sockfd, buffer, strlen(buffer));
            break;
        }

        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) { error("ERROR writing to socket"); }
    }

    shutdown(sockfd, 2);
    printf("Connection closed.\n");

    exit(0);
}

void receiver(int needs_arg)
{
    const int port = 5005;
    int sockfd, newsockfd;
    int client_length;
    int n;
    char buffer[256];
    struct sockaddr_in server_addr, client_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) { error("ERROR opening socket"); }

    // Set all values of the server_addr to zero
    bzero((char *) &server_addr, sizeof(server_addr));
    // Set address family
    server_addr.sin_family = AF_INET;
    // Convert port to network byte order and assign the port of the server
    server_addr.sin_port = htons(port);
    // Set the server address to the IP of the host
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) { error("ERROR binding"); }

    // Listen for connections
    listen(sockfd, 5);

    while (1)
    {
        bzero(buffer, 256);
        client_length = sizeof(client_addr);
        char client_nickname[256];

        // Accept the connection
        newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_length);
        if (newsockfd < 0) { error("ERROR accepting"); }

        // Get the nickname of the client
        n = read(newsockfd, client_nickname, 255);
        if (n < 0) { error("ERROR reading from socket while getting nickname"); }
        strtok(client_nickname, "\n");

        printf("Connection established with %s\n", client_nickname);

        // Zero the buffer
        bzero(buffer, 256);

        // Read the socket
        while (1)
        {
            // Zero the buffer
            bzero(buffer, 256);

            // Read the socket
            n = read(newsockfd, buffer, 255);
            if (n < 0) { error("ERROR reading from socket"); }

            // Print the output
            printf("\n<%s> %s", client_nickname, buffer);

            // Check for internal commands
            if (strcmp(buffer,"--EXIT--\n") == 0) { break; }
        }

        printf("Connection with %s closed.\n", client_nickname);
    }
}
