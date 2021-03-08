source=$(shell find *.cc)
object = $(patsubst %.cc, %.o, $(source))


server:$(source)
	g++ -std=c++11  $(source) -o server

clean:
	rm server
