#ifndef GENRE
    #define GENRE

    struct message 
    {
        int message_num;
        void* next;
        char* value;
    };

    struct gserver 
    {
        int start_message;
        int end_message;
        struct message* start;
        struct message* end;
    };

    void initServer();

    void moveHead();

    void addMessage(char* value);

    char* getMessageAtIndex(int message_num);

#endif