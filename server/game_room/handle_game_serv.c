#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "room_server.h"

int game_msg_parse(int sock){
    if(version_check(sock)<0){
        return -1;
    }
    switch(msg_type_check(sock)){
        case 3:
            break;
        default:
            return -1;
    }
    unsigned char msg[BUFFER_SIZE];
    memset(msg,0,sizeof(msg));
    char msg_length;
    int buf_len,used_len=0;
    int used_room,p_num;
    if(recv(sock,&msg_length,1,0)<=0){
        return -1;
    }
    while(1){
        buf_len = recv(sock,msg + used_len,msg_length,0);
        if(buf_len<=0){
        return -1;
        }else if(!(msg_length -= buf_len)){
        used_len += buf_len;
        break;
        }else{
        used_len += buf_len;
        }
    }
    sscanf(msg,"%d %d",&p_num,&used_room);
    switch(p_num){
        case 2:
            p2_room_return(used_room);
            break;
        default:
            return -1;
    }
    return 0;
}