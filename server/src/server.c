#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "genre_servers.h"
#include "server_thread.h"

int attemptReceive(long sock, void* message, size_t n);
int attemptSend(long sock, void* message, size_t n);
void* server_handler(void* sock);
short socketCreate();
int bindCreatedSocket(int hSocket);

int main() 
{
    int socket_desc = 0, clientLen = 0;
    long sock = 0;
    struct sockaddr_in client;
    pthread_t thread_t;
    pthread_t debug_t;

    initServer();
    initThreadList();

    socket_desc = socketCreate();
    if (socket_desc == -1) 
    {
        printf("Could not create socket\n");
        return 1;
    }

    if (bindCreatedSocket(socket_desc) < 0) 
    {
        printf("Failed to bind socket\n");
        return 1;
    }

    listen(socket_desc, 10);

    while(1) 
    {
        printf("Waiting for incoming connections...\n");
        clientLen = sizeof(struct sockaddr_in);
        sock = accept(socket_desc, (struct sockaddr*) &client, (socklen_t*) &clientLen);
        if (sock < 0) {
            printf("Accept failed\n");
            return 1;
        }

        printf("Connection succeeded! Creating thread to handle client\n");

        pthread_create(&thread_t, NULL, server_handler, (void*)sock);

        sleep(1);
        printf("Returning to ");
    }

    printf("Server shutting down\n");

    return 0;
}

int attemptReceive(long sock, void* message, size_t n) 
{
    if (recv(sock, message, n, 0) < 0) 
    {
        printf("Recieve failed in thread %d\n", pthread_self());
        return -1;
    }
    return 0;
}

int attemptSend(long sock, void* message, size_t n) 
{
    if (send(sock, message, n, 0) <= 0) 
    {
        printf("Send failed in thread %d\n", pthread_self());
        return -1;
    }
    return 0;
}

int sendMessage(long sock, char* message)
{
    // Send size of meesage prior to actual message
    int size = strlen(message);
    if(attemptSend(sock, &size, sizeof(int)) < 0)
    {
        // gracefully close thread
        printf("Failed to send size of string: %d\n");
        return -1;
    }

    // Send actual message
    if(attemptSend(sock, message, strlen(message)) < 0)
    {
        // gracefully close thread
        printf("Failed to send string: %s\n", message);
        return -2;
    }

    return 0;
}

int receiveMessage(long sock, char* message)
{
    // Get size of message prior to receiving the message
    int size = 0;
    if (attemptReceive(sock, &size, sizeof(int)) < 0)
    {
        // gracefully close thread
        printf("Failed to receive size of message from client\n");
        return -1;
    }

    int time_out = 10000;
    while (recv(sock, message, sizeof(message), MSG_PEEK) < size && time_out-- > 0);

    // if loop has ended by time out, report error
    if (time_out <= 0)
    {
        // gracefully close thread
        printf("Failed to receive message from client\n");
        return -2;
    }

    recv(sock, message, sizeof(message), 0);

    return 0;
}

void* server_handler(void* sock) 
{
    addThread(pthread_self());

    char message[MAX_MESSAGE_LENGTH] = "Welcome to the chat room! Now loading messages...\n";
    char client_message[MAX_MESSAGE_LENGTH];
    struct server_thread* thread = getThread(pthread_self());

    if (sendMessage((long)sock, message) < 0)
    {
        // gracefully close thread
        goto out;
    }

    printf("Thread %d: Sent welcome message...\n", pthread_self());

    char* quit_command = "/quit";
    while(1)
    {
        memset(client_message, 0, sizeof(client_message));
        if (thread->current_message != getEndMessageIndex())
        {
            printf("Sending message #%d...\n", thread->current_message);
            strcpy(message, getMessageAtIndex(thread->current_message));
            if (sendMessage((long)sock, message) == 0) thread->current_message = (thread->current_message + 1) % MAX_MESSAGES;
            else
            {
                // gracefully close thread due error
                goto out;
            }
        }
        else printf("Thread %d: No unread messages, now waiting to receive...\n", pthread_self());

        if (receiveMessage((long)sock, client_message) < 0)
        {
            // gracefully close thread due to error
            goto out;
        }
        if (strcmp(client_message, quit_command) == 0)
        {
            // gracefully close thread due to client quit command
            goto out;
        }
        printf("Client said: %s\n", client_message);
        addMessage(pthread_self(), client_message);
    }

out:
    removeThread(getThread(pthread_self()));

    printf("Terminating connection in thread %d\n\n", pthread_self());
    pthread_exit(NULL);
}

short socketCreate() 
{
    short hSocket = 0;
    printf("Created socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}

int bindCreatedSocket(int hSocket) 
{
    int iRetVal = -1;
    int clientPort = 12345;

    struct sockaddr_in remote = {0};

    remote.sin_family = AF_INET;

    remote.sin_addr.s_addr = htonl(INADDR_ANY);
    remote.sin_port = htons(clientPort);

    iRetVal = bind(hSocket, (struct sockaddr*) &remote, sizeof remote);

    return iRetVal;
}