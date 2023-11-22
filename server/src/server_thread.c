#include <stdlib.h>
#include <stdio.h>

#include "server_thread.h"
#include "genre_servers.h"

struct server_thread* server_head;
struct server_thread* server_tail;

extern struct gserver* main_server;

pthread_mutex_t server_list_lock;

void initHead() 
{
    server_head = NULL;
    pthread_mutex_init(&server_list_lock, NULL);
}

void addThread(pthread_t thread) 
{
    struct server_thread* s = malloc(sizeof(struct server_thread));
    s->thread = thread;
    s->current_message = main_server->start_message;
    s->next = NULL;

    pthread_mutex_lock(&server_list_lock);
    if (server_head == NULL) 
    {
        server_head = s;
        server_tail = s;
    }
    else 
    {
        server_tail->next = s;
        server_tail = s;
    }
    pthread_mutex_unlock(&server_list_lock);
}

void removeThread(struct server_thread* thread) 
{
    pthread_mutex_lock(&server_list_lock);
    if (server_head == thread) 
    {
        if (server_tail == thread) 
        {
            server_head = NULL;
            server_tail = NULL;
        }
        else 
        {
            server_head = server_head->next;
        }
    }
    else 
    {
        struct server_thread* temp = server_head;
        while(temp->next != thread) {temp = temp->next;}
        temp->next = ((struct server_thread*)(temp->next))->next;
    }
    pthread_mutex_unlock(&server_list_lock);
    free(thread);
}

struct server_thread* getThread(pthread_t thread) 
{
    struct server_thread* s = server_head;
    while (s->thread != thread && s != NULL) {s = s->next;}
    return s;
}