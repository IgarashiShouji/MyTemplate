#
#	Makefile
#

all: Objects mruby test

clean:
	/bin/rm -rf Objects mruby test default.rb.lock

mruby:
	/bin/bash get-github.sh

Objects:
	mkdir Objects

test: Objects/main.o
	g++ -o $@ $^ -L Objects -L mruby/build/host/lib -lmruby -lmystub -lboost_program_options

Objects/libmystub.a: Objects/mystub.o
	ar rcs $@ $^

Objects/mystub.o: Source/mystub.c Include/mystub.h
	gcc -c -I Include -o $@ $<

mruby/build/host/lib/libmruby.a: mruby Objects/libmystub.a mruby-mystub/src/mruby_mysub.c mruby-mystub/include/mruby/mruby_mystub/mruby_mystub.h
	bash build-mruby.sh

Objects/main.o: Source/main.cpp mruby/build/host/lib/libmruby.a
	g++ -c -I Include/ -I mruby/include/ -o $@ $<
