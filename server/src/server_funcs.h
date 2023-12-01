#ifndef SERVER_H
    #define SERVER_H

    int attemptReceive(long sock, void* message, size_t n);

    int attemptSend(long sock, void* message, size_t n);

    void* server_handler(void* sock);

    short socketCreate();
    
    int bindCreatedSocket(int hSocket);

#endif