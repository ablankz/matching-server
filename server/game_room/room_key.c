#include "room_server.h"
#include <stdlib.h>
#include <time.h>

void room_key_gen(char *room_key){
    int ex_key;
    int i;
    srand((unsigned int)time(NULL));
    for(i=0;i<ROOM_KEY_LEN;++i){
            ex_key = rand() % 93 + 33;
            *(room_key + i) = ex_key;
    }
    *(room_key + i) = 0;
}