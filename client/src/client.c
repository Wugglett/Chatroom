#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>

#include "client_funcs.h"

int main(int argc, char** argv)
{

    long hSocket = 0;
    struct sockaddr_in server;
    int sendToServer;
    char server_reply[200] = {0};
    pthread_t thread;
    
    openChatlog();

    hSocket = socketCreate();

    if (hSocket == -1) 
    {
        printf("Failed to create socket\n");
        return 1;
    }

    if (socketConnect(hSocket) < 0) 
    {
        printf("Failed to connect\n");
        return 1;
    }

    printf("Successfully connected to server!\n\n");

    // Wait for server to send greeting, then read and print the greeting
    socketReceive(hSocket, server_reply, 200);
    printf("%s", server_reply);

    pthread_create(&thread, NULL, client_handler, (void*)hSocket);

    char client_message[200] = {0};
    while(1)
    {
        printf("\"username\": ");

        // Clear the message string every loop
        memset(client_message, 0, sizeof(client_message));

        scanf("%s", client_message);
        // If client enters one of two quit commands then end connection
        socketSendStr(hSocket, client_message, strlen(client_message));
        if (strcmp(client_message, "/quit") == 0 || strcmp(client_message, "/exit") == 0) break; // break out of loop to close application
    }

    printf("Closing sending/main thread...\n");

    setCloseCheck();
    close(hSocket);
    closeChatlog();

    return 0;
}