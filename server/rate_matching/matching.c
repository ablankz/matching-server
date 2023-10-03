#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include "matching.h"

static int room_serv_connect(void);
static int init_sockaddr_in(struct sockaddr_in *sin);

int room_serv_sock;

int main(){
  if(room_serv_connect()<0){
    return -1;
  }
  queue_t queue;
  init_queue(&queue);
  sem_t sem;
  sem_init(&sem,0,1);
  send_t common_data={&queue,&sem};
  pthread_t tid;
  //error check
  int result;
  //setsockopt
  char opt=1;
  //accept
  int dstSock;
  struct sockaddr_in dstAddr;
  int dstAddrSize = sizeof(dstAddr);
  //listen
  struct sockaddr_in srcAddr;
  int srcSocket;
  //srcaddr init
  memset(&srcAddr, 0, sizeof(srcAddr));
  srcAddr.sin_port = htons(MATCHING_PORT);
  srcAddr.sin_family = AF_INET;
  srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  /* socket */
  srcSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(srcSocket == -1){
    printf("socket error\n");
    sem_destroy(&sem);
    close(room_serv_sock);
    return -1;
  }
  if(setsockopt(srcSocket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int))!=0){
    printf("setsockopt error\n");
    goto error;
  }
  /* bind */
  result = bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));
  if(result == -1){
    printf("bind error\n");
    goto error;
  }
  /* listen */
  result = listen(srcSocket, SOMAXCONN);
  if(result == -1){
    printf("listen error\n");
    goto error;
  }
  /*====================matching algolism start=========================*/
  pthread_create(&tid, NULL, serv_thread, &common_data);
  /*=====================================================================*/
  while(1){
     /* accept */
    dstSock = accept(srcSocket, (struct sockaddr *)&dstAddr, (socklen_t *)&dstAddrSize);
    if(dstSock == -1){
      continue;
    }
    cli_send_t *cli_data = (cli_send_t *)malloc(sizeof(cli_send_t));
    if(cli_data==NULL){
      goto error;
    }
    data_t *p_data = (data_t *)malloc(sizeof(data_t));
    if(p_data==NULL){
      free(cli_data);
      goto error;
    }
    cli_data->data = p_data;
    cli_data->ctos_sock = dstSock;
    cli_data->send_data = &common_data;

    pthread_create(&tid, NULL, cli_thread, cli_data);
  }
  error:
    close(srcSocket);
    sem_destroy(&sem);
    close(room_serv_sock);
    return 0;
}

int room_serv_connect(void){
  int ret=EXIT_FAILURE;
  struct sockaddr_in sin;
  if((room_serv_sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
      fprintf(stderr,"Failed to generate socket.\n");
      return -1;
  }
  if(init_sockaddr_in(&sin)<0){
      fprintf(stderr,"Failed to init address structure.\n");
      close(room_serv_sock);
      return -1;
  }

  struct sockaddr_in cli_sock;
  memset(&cli_sock,0,sizeof(cli_sock));
  cli_sock.sin_family = PF_INET;
  cli_sock.sin_port = htons(TO_ROOM_PORT);
  if(!inet_aton(TO_ROOM_ADDR,&(cli_sock.sin_addr))){
    close(room_serv_sock);
    return -1;
  }
  bind(room_serv_sock,(struct sockaddr *)&cli_sock,sizeof(cli_sock));

  if(connect(room_serv_sock,(struct sockaddr*)&sin,sizeof(sin))<0){
      fprintf(stderr,"Failed to connect to server.\n");
      close(room_serv_sock);
      return -1;
  }
  return 0;
}

int init_sockaddr_in(struct sockaddr_in *sin){
    memset(sin,0,sizeof(*sin));
    sin->sin_family=AF_INET;
    if(!inet_aton(ROOM_SERV_ADDR,&sin->sin_addr)){
        return -1;
    }
    sin->sin_port=htons(ROOM_SERV_PORT);
    return 0;
}