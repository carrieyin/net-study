object = server.o client.o
all:server cli

server:server.o
	gcc server.c -o server -Wall -g

cli:client.o
	gcc client.c -o cli -Wall -g

.PHONY:clean
clean:
	rm server cli $(object) *.dSYM
