CFLAGS=-g -Wall -pipe -I ./Include --input-charset=UTF-8 --exec-charset=UTF-8 -O3 -march=native
CPPFLAGS=$(CFLAGS) -std=c++20
LIBS=-L . -lboost_program_options -lpthread

TERGETS=mygrep
all: $(TERGETS)

clean:
	rm -rf $(TERGETS)

mygrep: main.cpp
	g++ $(CPPFLAGS) -o $@ $< $(LIBS)
