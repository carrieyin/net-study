CC = g++
object = wrapper.o server.o client.o 
all:server cli

server:server.o wrapper.o
	$(CC) wrapper.c server.c -o server -Wall -g

cli:client.o
	$(CC) client.c -o cli -Wall -g

.PHONY:clean
clean:
	- rm server cli  server $(object)
	rm  -rf *.dSYM
