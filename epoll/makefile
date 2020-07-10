
all:server

commonobj = ../common/wrapper.o
object = $(commonobj) server.o 
	

VPATH=../common

%.o:%.c
	gcc -c $< -o $@ 

server:$(object)
	gcc  $(object) -o server -Wall -g


.PHONY:clean
clean:
	- rm server cli  $(object)
	rm  -rf *.dSYM
	echo $(object)
