#ifndef MATCHING_SERVER_H
#define MATCHING_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MATCHING_PORT 2009

#define P_NUM 2
#define BUFFER_SIZE 256
#define FIRST_WATING_WIDTH 100
#define SECOND_WAITING_WIDTH 200
#define THIRD_WAITING_WIDTH 300
#define FINAL_WAITING_WIDTH 500
#define ROOM_KEY_LEN 20

#define VERSION_1 0
#define VERSION_2 1

#define ROOM_SERV_ADDR "127.0.0.1"
#define ROOM_SERV_PORT 3003
#define TO_ROOM_ADDR "127.0.0.1"
#define TO_ROOM_PORT 12009

typedef struct{
  int id;
  int rating;
  time_t start_time;
  int width;
  int *room_num; //room number
  int *room_id; //player id in room
  char *room_key;
  sem_t *user_sem;
}data_t;

typedef struct node_tag{
  data_t data;
  struct node_tag *next;
}node_t;

typedef struct{
  node_t head;
  node_t *tail;
}queue_t;

typedef struct{
  queue_t *queue;
  sem_t *sem; 
}send_t;

typedef struct{
  send_t *send_data;
  int ctos_sock;
  data_t *data; 
}cli_send_t;

// receive
int msg_type_check(int sock);
int version_check(int sock);
int connect_msg_parse(int sock,char *msg);
// stoc_process
void* cli_thread(void *cli_param);
// matching_algolism
void* serv_thread(void *serv_param);
// data_structure
node_t* insert_node(node_t* n, data_t *data);
node_t* insert_data(data_t* data, node_t* head);
void remove_node(node_t* n);
node_t* Nth_node(node_t* head, int n);
node_t* Nth_data(data_t* data, node_t* head);
void init_queue(queue_t *q); 
void enqueue(queue_t *q,data_t *data);
void dequeue(queue_t *q,data_t *data);

extern int room_serv_sock;

#endif /* end of MATCHING_SERVER_H */