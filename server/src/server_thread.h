#ifndef THREAD
    #define THREAD

    #include <pthread.h>

    struct server_thread 
    {
        pthread_t thread;
        int current_message;
        void* next;
    };

    void initHead();

    void addThread(pthread_t thread);

    void removeThread(struct server_thread* thread);
    
    struct server_thread* getThread(pthread_t thread);

#endif