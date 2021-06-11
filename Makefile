CFLAGS=-g -Wall -pipe -I ./Include --input-charset=UTF-8 --exec-charset=UTF-8 -O3 -march=native
CPPFLAGS=$(CFLAGS) -std=c++20
ifdef MSYSTEM
LIBS=-L . -lMyTemplate -lboost_program_options-mt -lboost_filesystem-mt -lpthread -lws2_32 -static
else
LIBS=-L . -lMyTemplate  -lboost_program_options -lpthread
endif

ifdef MSYSTEM
TERGETS=test.exe WorkerThread.exe TextFilter.exe SerialCotrol.exe comlist.exe
else
TERGETS=test.exe WorkerThread.exe TextFilter.exe SerialCotrol.exe
endif
all: Objects $(TERGETS)

clean:
	rm -rf $(TERGETS)

Objects:
	mkdir -p Objects


Objects/%.o: Source/%.cpp
	g++ $(CPPFLAGS) -c -o $@ $<

libMyTemplate.a: Objects/MyThread.o Objects/WorkerThread.o Objects/TextFilter.o Objects/SerialCotrol.o
	ar rcus $@ $^

test.exe: Objects/main.o libMyTemplate.a
	g++ $(CPPFLAGS) -o $@ $< $(LIBS)

WorkerThread.exe: Source/WorkerThread.cpp Include/WorkerThread.hpp libMyTemplate.a
	g++ $(CPPFLAGS) -o $@ $< -D_TEST_WORKER_THREAD $(LIBS)

TextFilter.exe: Source/TextFilter.cpp libMyTemplate.a
	g++ $(CPPFLAGS) -o $@ $< -D_TEST_TEXT_FILTER $(LIBS)

SerialCotrol.exe: Source/SerialCotrol.cpp libMyTemplate.a
	g++ $(CPPFLAGS) -o $@ $< -D_TEST_SERIAL $(LIBS)

comlist.exe: Source/ComList.cpp Include/ComList.hpp
	g++ $(CPPFLAGS) -static -o $@ $< -D_COM_LIST -lsetupapi -lksguid -lole32 -lwinmm -ldsound -liconv



Objects/main.o:         Source/main.cpp         Include/MyThread.hpp Include/WorkerThread.hpp
Objects/MyThread.o:     Source/MyThread.cpp     Include/MyThread.hpp
Objects/WorkerThread.o: Source/WorkerThread.cpp Include/WorkerThread.hpp
Objects/TextFilter.o:   Source/TextFilter.cpp   Include/WorkerThread.hpp
Objects/SerialCotrol.o: Source/SerialCotrol.cpp Include/WorkerThread.hpp
