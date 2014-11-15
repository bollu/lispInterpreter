CC = clang -lstdc++ -std=c++11  -I../src -g -Wall
all: run

build: compile

compile:
	$(CC) -c ../src/main.cpp -o ../build/main.o 

link: compile
		$(CC) ../build/main.o -o lisp

run: link
	@echo --
	@ ./lisp

clean:
	rm ../build/*
