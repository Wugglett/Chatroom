#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>

int close_check = 0;

short socketCreate() 
{
    short hSocket = 0;
    printf("Created socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}

int socketConnect(int hSocket) 
{
    int iRetVal = -1;
    int serverPort = 12345;

    struct sockaddr_in remote = {0};

    remote.sin_family = AF_INET;

    remote.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote.sin_port = htons(serverPort);

    iRetVal = connect(hSocket, (struct sockaddr*) &remote, sizeof remote);

    return iRetVal;
}

int socketSend(int hSocket, int Rqst, short lenRqst) 
{
    int shortRetval = -1;

    shortRetval = send(hSocket, &Rqst, lenRqst, 0);

    return shortRetval;
}

int socketSendStr(int hSocket, char* Rqst, short lenRqst) 
{
    int retval = -1;

    // Always send string size first
    int size = strlen(Rqst);
    retval = send(hSocket, &size, sizeof(size), 0);

    if (retval <= 0) return retval;

    // Then send actual string
    retval = send(hSocket, Rqst, lenRqst, 0);

    return retval;
}

int socketReceive(int hSocket, char* Rsp, short RvcSize) 
{
    int retval = -1;

    int size = 0;
    // Get size of data sent
    retval = recv(hSocket, &size, sizeof(size), 0);

    if (retval <= 0) return retval;

    // Peek at socket until full data is available for read
    int time_out = 10000;
    while((retval = recv(hSocket, Rsp, RvcSize, MSG_PEEK)) < size && time_out-- > 0);

    if (time_out <= 0)
    {
        printf("Terminating attemptReceive: time out reached\n");
    }

    recv(hSocket, Rsp, RvcSize, 0);

    return retval;
}

void* client_handler(void* sock)
{
    char server_reply[200];
    memset(server_reply, 0, sizeof(server_reply));
    while(!close_check)
    {
        socketReceive((long)sock, server_reply, sizeof(server_reply));
        printf("%s", server_reply);
    }
    printf("Closing receiving thread...\n");
}

int main(int argc, char** argv) 
{

    long hSocket = 0;
    struct sockaddr_in server;
    int sendToServer;
    char server_reply[200] = {0};
    pthread_t thread;

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
    memset(client_message, 0, sizeof(client_message));
    while(1)
    {
        printf("\"username\": ");
        scanf("%s", client_message);
        // If client enters one of two quit commands then end connection
        socketSendStr(hSocket, client_message, sizeof(client_message));
        if (strcmp(client_message, "/quit") == 0 || strcmp(client_message, "/exit") == 0) break; // break out of loop to close application
    }

    close_check = 1;

    close(hSocket);

    printf("Closing sending/main thread...\n");

    return 0;
}