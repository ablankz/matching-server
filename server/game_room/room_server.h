#ifndef ROOM_SERV_H
#define ROOM_SERV_H

#include <stdio.h>
#include <string.h>

#define P2_MAX_ROOM 1000
#define TO_GAMESERV_PORT 3002
#define TO_MATCHSERV_PORT 3003
#define BUFFER_SIZE 256

#define ROOM_KEY_LEN 20

#define GAME_SERV_ADDR "127.0.0.1"
#define GAME_SERV_PORT 11128

#define MATCH_SERV_ADDR "127.0.0.1"
#define MATCH_SERV_PORT 12009

#define VERSION_1 0
#define VERSION_2 1

int p2_used_room[P2_MAX_ROOM];

int p2_room_search(void);
void p2_room_return(int return_room);
int msg_type_check(int sock);
int version_check(int sock);
int game_msg_parse(int sock);
int match_msg_parse(int sock);
void room_key_gen(char *room_key);

#endif /* end of ROOM_SERV_H */