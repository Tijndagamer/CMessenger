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
#include <unistd.h>

// Global variables
char *recv_msg = NULL;
char *send_msg = NULL;
char connected_client_nickname[256];
char nickname[256];

char *msg = ".";

void sender(char *server_char);
void receiver(int needs_arg);
void output(int needs_arg);

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

    const int need_arg = 2; /* I need to pass an argument to the function used in pthread, so why not pass my lucky number?*/
    pthread_t sender_thread, receiver_thread, output_thread;
    int sender_thread_result, receiver_thread_result, output_thread_result;

    sender_thread_result = pthread_create(&sender_thread, NULL, sender, (void*) argv[1]);
    receiver_thread_result = pthread_create(&receiver_thread, NULL, receiver, (void*) need_arg);
    output_thread_result = pthread_create(&output_thread, NULL, output, (void*) need_arg);

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

    // Opening socket
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
        client_length = sizeof(client_addr);;

        // Accept the connection
        newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_length);
        if (newsockfd < 0) { error("ERROR accepting"); }

        // Get the nickname of the client
        n = read(newsockfd, connected_client_nickname, 255);
        if (n < 0) { error("ERROR reading from socket while getting nickname"); }
        strtok(connected_client_nickname, "\n");

        printf("Connection established with %s\n", connected_client_nickname);

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
            //printf("\n<%s> %s", connected_client_nickname, buffer);
            //recv_msg = buffer;
            msg = buffer;
//            printf("%s %s\n", msg, buffer);

            // Check for internal commands
            if (strcmp(buffer,"--EXIT--\n") == 0) { break; }
        }

        printf("Connection with %s closed.\n", connected_client_nickname);
    }
}

void output(int needs_arg)
{
    char *prev_send_msg, *prev_recv_msg;
    char prev_msg[256];

    printf(msg);
    //prev_msg = msg;
    strcpy(prev_msg, msg);

    while(1)
    {
        if(strcmp(prev_msg, msg) != 0)
        {
            printf("Got into if statement in output\n");
            printf("%s\n", msg);
            //prev_msg = msg;
            strcpy(prev_msg, msg);

            printf("%s %s \n", msg, prev_msg);

            // Sleep for 10ms
            usleep(10);
        }
    }

    // Receive msg printing

    //bzero(prev_recv_msg, 256);
    //bzero(prev_send_msg, 256);

    //printf("<%s> %s \n", connected_client_nickname, recv_msg);
    //prev_recv_msg = recv_msg;

    /*
    while(1)
    {
        if (!(strcmp(prev_recv_msg, recv_msg) == 0))
        {
            printf("<%s> %s\n", connected_client_nickname, recv_msg);
        }
    }
    */

    // Send msg printing
    /*
    while (1)
    {

    }
    */
}
