WARNINGS = -Weverything -Wno-c++98-compat -Wno-padded -Wno-c++98-compat-pedantic \
		   -Werror=old-style-cast \
		   -Werror=conversion \
		   -Werror=extra-semi
		   #-Werror=missing-prototypes

CC = clang -lstdc++ -std=c++11  -I../src -g $(WARNINGS)
all: run

build: compile

compile:
	$(CC) -c ../src/ast.cpp -o ../build/ast.o 
	$(CC) -c ../src/datum.cpp -o ../build/datum.o 
	$(CC) -c ../src/environment.cpp -o ../build/environment.o 
	$(CC) -c ../src/interpreter.cpp -o ../build/interpreter.o 
	$(CC) -c ../src/main.cpp -o ../build/main.o 
	$(CC) -c ../src/token.cpp -o ../build/token.o 

link: compile
		$(CC) ../build/ast.o \
			../build/datum.o \
			../build/environment.o \
			../build/interpreter.o \
			../build/token.o  \
			../build/main.o -o lisp

run: link
	@echo --
	@ ./lisp 

clean:
	rm ../build/*
