#ifndef THREAD_H
    #define THREAD_H

    #include <pthread.h>

    struct server_thread 
    {
        pthread_t thread;
        int current_message;
        void* next;
    };

    void initThreadList();

    void addThread(pthread_t thread);

    void removeThread(struct server_thread* thread);
    
    struct server_thread* getThread(pthread_t thread);

#endif