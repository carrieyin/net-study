
server:server.o
	gcc server.c -o server -Wall -g

cli:cli.o
	gcc client.c -o cli -Wall -g
