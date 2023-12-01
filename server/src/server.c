#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "genre_servers.h"
#include "server_thread.h"
#include "server_funcs.h"

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