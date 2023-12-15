#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>

static FILE* chatlog = NULL;

static int close_check = 0;

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
    uint64_t size = lenRqst;
    retval = send(hSocket, &size, sizeof(size), 0);

    if (retval <= 0) return retval;

    // Then send actual string
    retval = send(hSocket, Rqst, lenRqst, 0);
    // Writing to DEBUG file
    fprintf(chatlog, "%s\n", Rqst);
    

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

void openChatlog()
{
    chatlog = fopen("chatlog.txt", "a");
}

void closeChatlog()
{
    fclose(chatlog);
}

void setCloseCheck()
{
    close_check = 1;
}