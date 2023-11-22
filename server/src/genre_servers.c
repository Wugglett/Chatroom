#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "genre_servers.h"

extern struct gserver* main_server;

pthread_mutex_t main_server_lock;

void initServer() 
{
    main_server = malloc(sizeof(struct gserver));
    main_server->start_message = 0;
    main_server->end_message = 0;
    main_server->start = NULL;
    main_server->end = NULL;


    pthread_mutex_init(&main_server_lock, NULL);
}

void moveHead() 
{
    struct message* temp = main_server->start;
    pthread_mutex_lock(&main_server_lock);
    main_server->start = temp->next;
    main_server->start_message++;
    pthread_mutex_unlock(&main_server_lock);
    free(temp);
}

void addMessage(char* value) 
{
    struct message* m = malloc(sizeof(struct message));
    m->message_num = ++main_server->end_message;
    m->next = NULL;
    m->value = value;

    pthread_mutex_lock(&main_server_lock);
    if (main_server->start == NULL) 
    {
        main_server->start = m;
        main_server->end = m;
    }
    else 
    {
        ((struct message*)(main_server->end))->next = m;
        main_server->end = m;
    }
    pthread_mutex_unlock(&main_server_lock);

    main_server->end_message++;
}

char* getMessageAtIndex(int message_num) 
{
    struct message* m = main_server->start;
    while (m->message_num <= message_num) 
    {
        m = m->next;
    }
    return m->value;
}