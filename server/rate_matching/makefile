# Makefile for matching_server
matching_server: matching.o matching_algolism.o stoc_process.o receive.o data_structure.o
	gcc -Wall -o matching_server matching.o matching_algolism.o stoc_process.o receive.o data_structure.o -pthread
matching.o: matching.c
	gcc -c matching.c
matching_algolism.o: matching_algolism.c
	gcc -c matching_algolism.c
stoc_process.o: stoc_process.c
	gcc -c stoc_process.c
receive.o: receive.c
	gcc -c receive.c
data_structure.o: data_structure.c
	gcc -c data_structure.c