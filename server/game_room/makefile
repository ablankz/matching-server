# Makefile for game_room_server
game_room_server: room_ope.o handle_serv.o handle_game_serv.o handle_match_serv.o room_key.o
	gcc -Wall -o game_room_server room_ope.o handle_serv.o handle_game_serv.o handle_match_serv.o room_key.o -pthread
room_ope.o: room_ope.c
	gcc -c room_ope.c
handle_serv.o: handle_serv.c
	gcc -c handle_serv.c
handle_game_serv.o: handle_game_serv.c
	gcc -c handle_game_serv.c
handle_match_serv.o: handle_match_serv.c
	gcc -c handle_match_serv.c
room_key.o: room_key.c
	gcc -c room_key.c