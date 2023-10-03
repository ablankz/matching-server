#include "matching.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int tocli_reply(int fd,int room_num,int room_id,char *room_key);

void* cli_thread(void *cli_param){
  cli_send_t *cli_data = (cli_send_t *)cli_param;
  data_t *p_data = cli_data->data;
  int sock = cli_data->ctos_sock;
  char buf[BUFFER_SIZE];
  char room_key[ROOM_KEY_LEN];
  int room_num;
  int room_id;
  sem_t cli_sem;
  sem_init(&cli_sem,0,0);
  memset(buf,0,sizeof(buf));
  if(version_check(sock)<0){
    goto close;
  }
  if(msg_type_check(sock)){
    goto close;
  }
  if(connect_msg_parse(sock,buf)<0){
    goto close;
  }
  sscanf(buf,"%d %d",&(cli_data->data->id),&(cli_data->data->rating));
  cli_data->data->start_time = time(NULL);
  cli_data->data->width = FIRST_WATING_WIDTH;
  cli_data->data->room_key = room_key;
  cli_data->data->room_num = &room_num;
  cli_data->data->room_id = &room_id;
  cli_data->data->user_sem = &cli_sem;
  // queue ope sem lock
  sem_wait(cli_data->send_data->sem);
  enqueue(cli_data->send_data->queue,cli_data->data);
  sem_post(cli_data->send_data->sem);

  /*matching success*/
  sem_wait(&cli_sem);
  printf("room_num::%d room_id::%d room_key::%s\n",*p_data->room_num,*p_data->room_id,p_data->room_key);
  if(tocli_reply(sock,*p_data->room_num,*p_data->room_id,p_data->room_key)<0){
    goto close;
  }

close:
  sem_destroy(&cli_sem);
  free(p_data);
  free(cli_data);
  close(sock);
  return cli_param;
}

int tocli_reply(int fd,int room_num,int room_id,char *room_key){
	unsigned char req[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	int length;
	memset(req,0,sizeof(req));
  memset(buffer,0,sizeof(buffer));
  snprintf(buffer,BUFFER_SIZE,"%d %d %s",room_num,room_id,room_key);
  req[0] = VERSION_1;
  req[1] = VERSION_2; //version:0.1
  req[2] = 2; //msg_type:room request
  req[3] = (char)strlen(buffer) + 1; //sizeof payload
  req[4] = '\0';
  strncat(&req[4],buffer,strlen(buffer)+1);
  length = strlen(buffer)+5;
	if(send(fd,req,length,0)<0){
      printf("message send error\n");
      return -1;
  }
	return 0;
}