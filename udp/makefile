
all:server client

server:server.o
	gcc -o server server.c

client:client.c
	gcc -o client client.c


clean:
	-rm *.o 
	-rm server
	-rm client
