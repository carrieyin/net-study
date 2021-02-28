CC = g++
source = wrapper.c server.c
sourcec = client.cpp
all:server cli

server:$(object)
	$(CC)  wrapper.c server.c -o server -Wall -g

cli:client.o
	$(CC) client.cpp -o cli -Wall -g

.PHONY:clean
clean:
	- rm server cli  server $(object)
	rm  -rf *.dSYM
