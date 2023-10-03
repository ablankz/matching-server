#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "room_server.h"

int version_check(int sock){
  char version[2];
  memset(version,0,sizeof(version));
  int buf_len;
  int length=2,used_len=0;
  while(1){
    buf_len = recv(sock,version + used_len,length,0);
    if(buf_len<=0){
      return -1;
    }else if(!(length -= buf_len)){
      break;
    }else{
      used_len += buf_len;
    }
  }
  if(version[0] != VERSION_1 || version[1] != VERSION_2){
    return -1;
  }
  return 0;
}

int msg_type_check(int sock){
  char msg_type;
  if(recv(sock,&msg_type,1,0)<=0){
    return -1;
  }
  return msg_type;
}