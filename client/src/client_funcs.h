#ifndef CLIENT_H
    #define CLIENT_H

    short socketCreate();

    int socketConnect(int hSocket);

    int socketSend(int hSocket, int Rqst, short lenRqst);

    int socketSendStr(int hSocket, char* Rqst, short lenRqst);

    int socketReceive(int hSocket, char* Rsp, short RvcSize);

    void* client_handler(void* sock);

    void openChatlog();

    void closeChatlog();

    void setCloseCheck();

#endif