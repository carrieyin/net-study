object = wrapper.o server.o client.o 
all:server cli

server:server.o wrapper.o
	gcc  wrapper.c server.c -o server -Wall -g

cli:client.o
	gcc client.c -o cli -Wall -g

.PHONY:clean
clean:
	- rm server cli  server $(object)
	rm  -rf *.dSYM
