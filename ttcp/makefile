commonobj=$(shell find comm/*.cc)
sources=$(commonobj) echo.cc
sourcec=$(commonobj) echo_client.cc
obs = $(patsubst %.cc, %.o, $(sources))
obc = $(patsubst %.cc, %.o, $(sourcec))
objects = $(notdir $(obs))
objectc = $(notdir $(obc))
head = comm
VPATH = ./
all: server client 

$(objects):$(sources)
	g++ -std=c++11 -c $^   -I$(head)
server:$(objects)
	g++  $(objects)  -o server

$(objectc):$(sourcec)
	g++ -std=c++11 -c $^   -I$(head)
client:$(objectc)
	g++  $(objectc)  -o client
clean:
	rm server
	rm client
	rm $(objects)
