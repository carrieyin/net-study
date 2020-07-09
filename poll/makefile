object = wrapper.o pollserver.o client.o 
all:server cli

server:pollserver.o wrapper.o
	gcc  wrapper.c pollserver.c -o server -Wall -g

cli:client.o
	gcc client.c -o cli -Wall -g

.PHONY:clean
clean:
	- rm server cli  $(object)
	rm  -rf *.dSYM
