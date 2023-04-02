#
#  Makefile
#

all: objects mruby test

clean:
	/bin/rm -rf objects mruby test default.rb.lock

mruby:
	/bin/bash script/get-mruby.sh

objects:
	mkdir objects

test: objects/main.o
	g++ -o $@ $^ -L objects -L mruby/build/host/lib -lmruby -lmystub -lboost_program_options

objects/libmystub.a: objects/mystub.o
	ar rcs $@ $^

objects/mystub.o: source/mystub.c include/mystub.h
	gcc -c -I include -o $@ $<

mruby/build/host/lib/libmruby.a: mruby objects/libmystub.a mruby-mystub/src/mruby_mysub.c mruby-mystub/include/mruby/mruby_mystub/mruby_mystub.h default.rb
	bash script/build-mruby.sh

source/help.c: mruby/build/host/lib/libmruby.a
	./mruby/bin/mrbc -s -Bmruby_help -o source/help.c source/help.rb

objects/main.o: source/main.cpp source/help.c mruby/build/host/lib/libmruby.a
	g++ -c -I include/ -I mruby/include/ -o $@ $<
