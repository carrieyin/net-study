CC = g++
CFLAGS = -std=c++11
source = wrapper.cpp server.cpp 
sourcec = client.cpp
objects = server.o wrapper.o
objectc = client.o

all:server cli

$(objects):$(source)
	$(CC) $(CFLAGS) -c -std=c++11 $(source) 

server:$(objects)
	$(CC)   $(objects) -o server -Wall -g 
	
cli:$(objectc)
	$(CC) $(CFLAGS) client.cpp -o cli -Wall -g

.PHONY:clean
clean:
	- rm server cli  server $(object)
	rm  -rf *.dSYM
	rm *.o
