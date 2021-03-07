CC = g++
CFLAGS = -std=c++11
source = wrapper.cpp server.cpp 
sourcec = client.cpp
objects = $(patsubst %.cpp, %.o, $(source))
objectc = $(patsubst %.cpp, %.o, $(sourcec))
headers = ./head
all:server cli

$(objects):$(source)
	$(CC) $(CFLAGS) -c  $(source) -I$(headers)

server:$(objects)
	$(CC)   $(objects) -o server -Wall -g 
	
cli:$(objectc)
	$(CC) $(CFLAGS) client.cpp -o cli -Wall -g

.PHONY:clean
clean:
	- rm server cli  server $(object)
	rm  -rf *.dSYM
	rm *.o
