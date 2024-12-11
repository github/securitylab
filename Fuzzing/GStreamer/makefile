CPPFLAGS = -g -O2

SRC = tree.cc labeler/MP4.cc
INC = ./ ./labeler

all: generator

generator: $(SRC) main.cc
	g++ main.cc -I./ -I./labeler $(SRC) $(CPPFLAGS) -o generator
