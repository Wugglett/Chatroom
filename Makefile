SERVER_OBJ = ./server/build/server.o ./server/build/genre_servers.o ./server/build/server_thread.o

CLIENT_OBJ = client/build/client.o

all: ./server ./client

./client: $(CLIENT_OBJ)
	gcc $(CLIENT_OBJ) -o ./client

./server: $(SERVER_OBJ)
	gcc $(SERVER_OBJ) -o ./server -lpthread

./server/build/server.o: server/src/server.c
	gcc -c server/src/server.c -o server/build/server.o

./server/build/genre_servers.o: server/src/genre_servers.c
	gcc -c server/src/genre_servers.c -o server/build/genre_servers.o

./server/build/server_thread.o: server/src/server_thread.c
	gcc -c server/src/server_thread.c -o server/build/server_thread.o
	
./client/build/client.o: client/src/client.c
	gcc -c client/src/client.c -o client/build/client.o

clean:
	rm -rf ./client.exe ./server.exe ./client/build/*.o ./server/build/*.o