#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include "room_server.h"

static int gen_reply(unsigned char *req,int blank_room,char *key);

int match_msg_parse(int sock){
    if(version_check(sock)<0){
        return -1;
    }
    switch(msg_type_check(sock)){
        case 1:
            break;
        default:
            return -1;
    }
    unsigned char msg[BUFFER_SIZE];
    memset(msg,0,sizeof(msg));
    char msg_length;
    int buf_len,used_len=0;
    int p_num,blank_room;
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
    sscanf(msg,"%d",&p_num);
    switch(p_num){
        case 2:
            blank_room = p2_room_search();
            break;
        default:
            return -1;
    }

    char room_key[20];
    room_key_gen(room_key);
    buf_len = gen_reply(msg,blank_room,room_key);
    if(send(sock,msg,buf_len,0)<0){
        printf("message send error\n");
        return -1;
    }
    fprintf(stderr,"p_num : %d room_num : %d room_key : %s\n",p_num,blank_room,room_key);
    return 0;
}


int gen_reply(unsigned char *req,int blank_room,char *key){
    char buffer[BUFFER_SIZE];
    memset(buffer,0,sizeof(buffer));
    snprintf(buffer,BUFFER_SIZE,"%d %s",blank_room,key);
    req[0] = VERSION_1;
    req[1] = VERSION_2; //version:0.1
    req[2] = 2; //msg_type:macth request
    req[3] = (char)strlen(buffer) + 1; //sizeof payload
    req[4] = '\0';
    strncat(&req[4],buffer,strlen(buffer)+1);
    return strlen(buffer)+5;
}