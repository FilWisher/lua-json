cc=gcc
cflags=-Wall -Wextra -pedantic -std=c99 -I/usr/includes/lua5.2
link=-llua5.2

default: json.so

lex.o: lex.c lex.h
	$(cc) $(cflags) lex.c -c -o lex.o

parse.o: parse.c parse.h
	$(cc) $(cflags) parse.c -c -o parse.o

json.o: json.c json.h
	$(cc) $(cflags) -fPIC $(link) json.c -c -o json.o
	
json.so: json.o parse.o lex.o
	$(cc) $(cflags) -shared json.o parse.o lex.o -o json.so
	
test:
	lua ./test.lua
	
clean:
	-rm -rf json.so json.o parse.o lex.o
