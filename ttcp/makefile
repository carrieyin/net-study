sources=$(shell find comm/*.cc) echo.cc
ob = $(patsubst %.cc, %.o, $(sources))
objects = $(notdir $(ob))
head = comm
VPATH = ./
all: server 

$(objects):$(sources)
	g++ -std=c++11 -c $^   -I$(head)
server:$(objects)
	g++  $(objects)  -o server

clean:
	rm server
	rm $(objects)
