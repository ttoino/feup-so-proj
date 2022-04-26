# -*- Makefile -*-

CC := gcc

all: phrases/phrases addmx/addmx cypher/cypher

phrases/phrases: ./phrases/main.c
	$(CC) ./phrases/main.c -o ./phrases/phrases

addmx/addmx: ./addmx/main.c
	$(CC) ./addmx/main.c -o ./addmx/addmx

cypher/cypher: ./cypher/main.c
	$(CC) ./cypher/main.c -o ./cypher/cypher

test: all
	sh ./test.sh

clean:
	rm -f phrases/phrases addmx/addmx cypher/cypher
