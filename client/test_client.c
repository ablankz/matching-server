#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 256
#define SERVER_ADDR "127.0.0.1"
#define MATCHING_SERV_PORT 2009
#define GAME_SERV_PORT 1128

#define ROOM_KEY_LEN 20

#define VERSION_1 0
#define VERSION_2 1

typedef struct{
    int id;
    int rating;
    int room_num; /* room number */
    int room_id; /* player id in room */
    char room_key[ROOM_KEY_LEN];
}data_t;

static int init_sockaddr_in(struct sockaddr_in *sin,int port);
static int match_client_process(int fd,data_t *data);
static int gen_match_req(data_t *user_data,unsigned char *req);
static int from_match_resp(int fd,data_t *data);
static int msg_type_check(int sock);
static int version_check(int sock);

int main(void){
    data_t test_data;
    printf("id: ");
    scanf("%d",&test_data.id);
    printf("rating: ");
    scanf("%d",&test_data.rating);

    int fd;
    struct sockaddr_in sin;
    if((fd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
        fprintf(stderr,"Failed to generate matching server socket.\n");
        return -1;
    }
    if(init_sockaddr_in(&sin,MATCHING_SERV_PORT)<0){
        fprintf(stderr,"Failed to init matching server address structure.\n");
        close(fd);
        return -1;
    }
    if(connect(fd,(struct sockaddr*)&sin,sizeof(sin))<0){
        fprintf(stderr,"Failed to connect to matching server.\n");
        close(fd);
        return -1;
    }
    if(match_client_process(fd,&test_data)<0){
        fprintf(stderr,"matching error\n");
        close(fd);
        return -1;
    }
    close(fd);

    printf("room_num::%d room_id::%d room_key::%s\n",test_data.room_num,test_data.room_id,test_data.room_key);
    /*if((fd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
        fprintf(stderr,"Failed to generate game server socket.\n");
        return -1;
    }
    if(init_sockaddr_in(&sin,GAME_SERV_PORT)<0){
        fprintf(stderr,"Failed to init game server address structure.\n");
        close(fd);
        return -1;
    }
    if(game_client_process(fd,&test_data)<0){
        fprintf(stderr,"matching error\n");
        close(fd);
        return -1;
    }*/
    
    return 0;
}

int init_sockaddr_in(struct sockaddr_in *sin,int port){
    memset(sin,0,sizeof(*sin));
    sin->sin_family=AF_INET;
    if(!inet_aton(SERVER_ADDR,&sin->sin_addr)){
        return -1;
    }
    sin->sin_port=htons(port);
    return 0;
}

int match_client_process(int fd,data_t *data){
    unsigned char match_req[BUFFER_SIZE]="";
    int length = gen_match_req(data,match_req);
    if(send(fd,match_req,length,0)<0){
        return -1;
    }
    if(from_match_resp(fd,data) < 0){
        return -1;
    }
    return 0;
}

int from_match_resp(int fd,data_t *data){
	if(version_check(fd)<0){
        return -1;
    }
    switch(msg_type_check(fd)){
        case 2:
            break;
        default:
            return -1;
    }
    unsigned char msg[BUFFER_SIZE];
    memset(msg,0,sizeof(msg));
    char msg_length;
    int buf_len,used_len=0;
    if(recv(fd,&msg_length,1,0)<=0){
        return -1;
    }
    while(1){
        buf_len = recv(fd,msg + used_len,msg_length,0);
        if(buf_len<=0){
            return -1;
        }else if(!(msg_length -= buf_len)){
            used_len += buf_len;
            break;
        }else{
            used_len += buf_len;
        }
    }
    sscanf((char *)msg,"%d %d %s",&data->room_num,&data->room_id,data->room_key);
    return 0;
}

int gen_match_req(data_t *user_data,unsigned char *req){
    char buffer[BUFFER_SIZE];
    memset(buffer,0,sizeof(buffer));
    snprintf(buffer,BUFFER_SIZE,"%d %d",user_data->id,user_data->rating);
    req[0] = VERSION_1;
    req[1] = VERSION_2; //version:0.1
    req[2] = 0; //msg_type:macth request
    req[3] = (char)strlen(buffer) + 1; //sizeof payload
    req[4] = '\0';
    strncat((char *)&req[4],buffer,strlen(buffer)+1);
    return strlen(buffer)+5;
}

int version_check(int sock){
  char version[2];
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