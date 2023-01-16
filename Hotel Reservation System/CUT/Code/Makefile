TARGET=client server 
CC=gcc 

server: obj/server.o 
	$(CC) obj/server.o -o server -lpthread

client: obj/client.o
	$(CC) obj/client.o -o client

client.o: src/client.c
	$(CC)  src/client.c

server.o: src/server.c
	$(CC)  src/server.c 
clean:
	$(RM) $(TARGET)
