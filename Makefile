TARGET=test.exe

CFLAGS=-g -Wall -pipe -I ./Include --input-charset=UTF-8 --exec-charset=UTF-8 -O3 -march=native
CPPFLAGS=$(CFLAGS) -std=c++14

all: Objects $(TARGET) WorkerThread.exe

clean:
	rm -rf $(TARGET) libMyTemplate.a Objects WorkerThread.exe

libMyTemplate.a: Objects/MyThread.o Objects/WorkerThread.o
	ar rcus $@ $^

$(TARGET): Objects/main.o libMyTemplate.a Objects
	g++ $(CPPFLAGS) -static -o $@ $< -L . -lMyTemplate -lpthread -lws2_32

Objects:
	mkdir -p Objects

Objects/%.o: Source/%.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

WorkerThread.exe: Source/WorkerThread.cpp Include/WorkerThread.hpp
	g++ $(CPPFLAGS) -static -o WorkerThread.exe Source/WorkerThread.cpp -D_TEST_MAIN -lpthread -lws2_32

Objects/main.o:     Source/main.cpp     Include/MyThread.hpp
Objects/MyThread.o: Source/MyThread.cpp Include/MyThread.hpp
Objects/WorkerThread.o:  Source/WorkerThread.cpp  Include/WorkerThread.hpp
