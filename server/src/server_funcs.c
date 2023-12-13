#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "server_funcs.h"
#include "genre_servers.h"
#include "server_thread.h"

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

int checkSocket(long sock)
{
    int rv = 0;
    char m[1024];
    rv = recv(sock, (void*)m, sizeof(m), MSG_PEEK);
    return rv;
}

int attemptReceive(long sock, void* message, size_t n, int* rv) 
{
    if ((*rv = recv(sock, message, n, 0)) < 0) 
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

int receiveMessage(long sock, char* message, int* rv)
{
    // Get size of message prior to receiving the message
    int size = 0;
    if (attemptReceive(sock, &size, sizeof(int), rv) < 0)
    {
        // gracefully close thread
        printf("Failed to receive size of message from client\n");
        return -1;
    }

    int time_out = 10000;
    while ((*rv = recv(sock, message, sizeof(message), MSG_PEEK)) < size && time_out-- > 0);

    // if loop has ended by time out, report error
    if (time_out <= 0)
    {
        // gracefully close thread
        printf("Failed to receive message from client\n");
        return -2;
    }

    *rv = recv(sock, message, sizeof(message), 0);

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

    char* quit_command[2] = {"/quit", "/exit"};
    while(1)
    {
        memset(client_message, 0, sizeof(client_message));
        if (thread->current_message != getEndMessageIndex())
        {
            printf("Sending message #%d...\n", thread->current_message);
            struct server_message sm = getMessageAtIndex(thread->current_message);

            // Check that the message is not one from this user
            if (sm.thread != pthread_self())
            {
                strcpy(message, sm.message);
                if (sendMessage((long)sock, message) < 0)
                {
                    // gracefully close thread due error
                    goto out;
                }
            }
            else
            { 
                printf("Not sending message %d: Thread is owner of message\n", thread->current_message);
            }

            thread->current_message = (thread->current_message + 1) % MAX_MESSAGES;
        }
        else
        {
            printf("Thread %d: No unread messages, now waiting to receive...\n", pthread_self());
            // If there is data to be read then start message reception
            if (checkSocket((long)sock) > 0)
            {
                int rv = 0;
                if (receiveMessage((long)sock, client_message, &rv) < 0)
                {
                    // gracefully close thread due to error
                    goto out;
                }
                if (strcmp(client_message, quit_command[0]) == 0 || strcmp(client_message, quit_command[1]) == 0)
                {
                    // gracefully close thread due to client quit command
                    printf("Client %d entered quit command...\n", pthread_self());
                    goto out;
                }
                if (rv > 0)
                {
                    printf("Client said: %s and size was %d\n", client_message, rv);
                    addMessage(pthread_self(), client_message);
                }
            }
            else
            {
                printf("Nothing in socket buffer to read...\n");
                sleep(1);    
            }
        }
    }

out:
    removeThread(getThread(pthread_self()));

    printf("Terminating connection in thread %d\n\n", pthread_self());
    pthread_exit(NULL);
}