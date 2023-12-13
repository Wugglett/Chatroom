#ifndef GENRE_H
    #define GENRE_H

    #include <pthread.h>

    #define MAX_MESSAGES 200
    #define MAX_MESSAGE_LENGTH 200

    struct server_message
    {
        pthread_t thread;
        char message[MAX_MESSAGE_LENGTH];
    };

    struct message_handler
    {
        int start;
        int end;
        struct server_message messages[MAX_MESSAGES];
    };

    void initServer();

    void addMessage(pthread_t thread, char* value);

    struct server_message getMessageAtIndex(int index);

    int getEndMessageIndex();

    int getStartMessageIndex();

#endif