# -*- Makefile -*-

CC := gcc

all: phrases

phrases: ./phrases/main.c
	$(CC) ./phrases/main.c -o ./phrases/phrases
