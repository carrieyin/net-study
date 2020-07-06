object = wrapper.o server.o client.o 
all:server

server:server.o wrapper.o
	gcc  wrapper.c server.c -o server -Wall -g


.PHONY:clean
clean:
	- rm server   $(object)
	rm  -rf *.dSYM
