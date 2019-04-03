TARGET=test.exe

CFLAGS=-g -Wall -pipe -I ./Include --input-charset=UTF-8 --exec-charset=UTF-8 -O3 -march=native
CPPFLAGS=$(CFLAGS) -std=c++14

all: Objects $(TARGET) MyThread.exe

clean:
	rm -rf $(TARGET) libMyTemplate.a Objects MyThread.exe

libMyTemplate.a: Objects/MyThread.o
	ar rcus $@ $^

$(TARGET): Objects/main.o libMyTemplate.a Objects
	g++ $(CPPFLAGS) -o $@ $< -L . -lMyTemplate -lpthread

Objects:
	mkdir -p Objects

Objects/%.o: Source/%.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

MyThread.exe:       Source/MyThread.cpp Include/MyThread.hpp
	g++ $(CPPFLAGS) -o MyThread.exe Source/MyThread.cpp -D_TEST_MAIN -lpthread

Objects/main.o:     Source/main.cpp     Include/MyThread.hpp
Objects/MyThread.o: Source/MyThread.cpp Include/MyThread.hpp

