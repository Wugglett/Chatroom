#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "genre_servers.h"

static struct message_handler server_messages;

pthread_mutex_t main_server_lock;

void initServer() 
{
    pthread_mutex_init(&main_server_lock, NULL);

    server_messages.start = 0;
    server_messages.end = 0;
    for (int i = 0; i < MAX_MESSAGES; i++)
    {
        struct server_message sm = {0, {'N', 'U', 'L', 'L'}};
        server_messages.messages[i] = sm;
    }
}

void addMessage(pthread_t thread, char* value)
{
    pthread_mutex_lock(&main_server_lock);

    struct server_message m;
    m.thread = thread;
    strcpy(m.message, value);

    server_messages.messages[server_messages.end] = m;
    server_messages.start = server_messages.end;
    server_messages.end = (server_messages.end + 1) % MAX_MESSAGES;

    pthread_mutex_unlock(&main_server_lock);
}

char* getMessageAtIndex(int index)
{
    pthread_mutex_lock(&main_server_lock);
    printf("Fetching message at index %d: %s\n", index, server_messages.messages[index].message);
    return server_messages.messages[index].message;
    pthread_mutex_unlock(&main_server_lock);
}

int getEndMessageIndex()
{
    return server_messages.end;
}

int getStartMessageIndex()
{
    return server_messages.start;
}

void* debugMessageList(void* v)
{
    int i = 0;
    while (1)
    {
        printf("Message %d: %s\n", i+1, server_messages.messages[i].message);
        i = (i + 1) % MAX_MESSAGES;
        sleep(1);
    }
}