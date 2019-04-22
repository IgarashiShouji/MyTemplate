TARGET=test.exe

CFLAGS=-g -Wall -pipe -I ./Include --input-charset=UTF-8 --exec-charset=UTF-8 -O3 -march=native
ifdef MSYSTEM
LIBS=-static -L . -lMyTemplate -lboost_program_options-mt -lpthread -lws2_32
else
LIBS=-L . -lMyTemplate  -lboost_program_options -lboost_regex -lboost_system -lpthread
endif
CPPFLAGS=$(CFLAGS) -std=c++14

ifdef MSYSTEM
all: Objects $(TARGET) WorkerThread.exe TextFilter.exe ComList.exe
else
all: Objects $(TARGET) WorkerThread.exe TextFilter.exe
endif

clean:
	rm -rf $(TARGET) libMyTemplate.a Objects WorkerThread.exe TextFilter.exe

Objects:
	mkdir -p Objects

libMyTemplate.a: Objects Objects/*.o
	ar rcus $@ Objects/*.o

WorkerThread.exe: Source/WorkerThread.cpp Include/WorkerThread.hpp libMyTemplate.a
	g++ $(CPPFLAGS) -o $@ $< -D_TEST_WORKER_THREAD $(LIBS)

TextFilter.exe: Source/TextFilter.cpp libMyTemplate.a
	g++ $(CPPFLAGS) -o $@ $< -D_TEST_TEXT_FILTER $(LIBS)

ifdef MSYSTEM
ComList.exe: Source/ComList.cpp Include/ComList.hpp
	g++ $(CPPFLAGS) -o $@ $< -D_COM_LIST -lsetupapi -lksguid -lole32 -lwinmm -ldsound -liconv
endif

$(TARGET): Objects/main.o libMyTemplate.a
	g++ $(CPPFLAGS) -o $@ $< $(LIBS)

Objects/%.o: Source/%.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

Objects/main.o:         Source/main.cpp         Include/MyThread.hpp Include/WorkerThread.hpp
Objects/MyThread.o:     Source/MyThread.cpp     Include/MyThread.hpp
Objects/WorkerThread.o: Source/WorkerThread.cpp Include/WorkerThread.hpp
Objects/TextFilter.o:   Source/TextFilter.cpp   Include/WorkerThread.hpp
