cc=gcc
cflags=-Wall -Wextra -pedantic -std=c99 -g -I/usr/includes/lua5.2
link=-llua5.2

default: test #json.so parse_test

lex.o: lex.c lex.h
	$(cc) $(cflags) lex.c -c -o lex.o

lex_test: lex_test.c lex.o
	$(cc) $(cflags) lex_test.c lex.o -o lex_test

parse.o: parse.c parse.h
	$(cc) $(cflags) parse.c -c -o parse.o

parse_test: parse_test.c parse.o lex.o
	$(cc) $(cflags) $(link) parse_test.c lex.o parse.o -o parse_test

json.o: json.c
	$(cc) $(cflags) -fPIC $(link) json.c -c -o json.o

json.so: json.o parse.o lex.o
	$(cc) $(cflags) -shared json.o parse.o lex.o -o json.so

test: parse_test json.so
	#./parse_test
	#./lex_test
	lua ./test.lua
	
clean:
	-rm -rf json.so json.o parse.o lex.o lex_test parse_test
