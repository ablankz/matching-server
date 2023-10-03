#include "matching.h"
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static void Print_Status(node_t* data);
static void Print_Status_All(node_t* head);
static void Search(node_t* head, time_t now);
static int Search_Nsec(node_t* target);
static void Width_Update(node_t* p, time_t now);
static void mass_insert(send_t *sync_data,node_t *head); 
static int room_req(void);
static int room_resp(char *room_key);

void* serv_thread(void *serv_param){
    send_t *sync_data = (send_t *)serv_param;
	node_t head;
	head.data.rating = -1;
    head.data.id = -1;
    head.next = NULL;
    time_t now;

    while(1){
        now = time(NULL);
        mass_insert(sync_data,&head);
        sleep(1);
		Print_Status_All(&head);
		printf("\n");
		Search(&head, now);
    }
    return serv_param;
}

void Print_Status(node_t* data) {
	printf("ID : %03d   Rating : %5d   Wating_Time : %d\n",data->data.id,data->data.rating,(int)(time(NULL) - data->data.start_time));
}

void Print_Status_All(node_t* head) {
    node_t *p;
	for (p = head->next; p != NULL; p = p->next) {
		Print_Status(p);
	}
}

void mass_insert(send_t *sync_data,node_t *head){
    data_t tmp;
    while(sync_data->queue->head.next != NULL){
        sem_wait(sync_data->sem);
        dequeue(sync_data->queue,&tmp);
        sem_post(sync_data->sem);
        insert_data(&tmp,head);
    }
}

void Search(node_t* head, time_t now) {
	node_t* tmp = head, *p;
	char flag = 0;
	int ret;
	for (p = head->next; p != NULL && p->next != NULL; p = p->next) {
		Width_Update(p, now);
		if (ret=Search_Nsec(tmp)) {
			flag = 1;
		}else if(ret < 0){
			return;
		}
		if (flag) {
			p = tmp;
			flag = 0;
		}else {
			tmp = p;
		}
	}
	return;
}

int Search_Nsec(node_t * target) {
	if (target->next->data.rating + target->next->data.width >= target->next->next->data.rating) {
		data_t player[P_NUM];
		char room_key[ROOM_KEY_LEN];
		player[0] = target->next->data;
		player[1] = target->next->next->data;
		printf("%3d - %3d\n", player[0].id,player[1].id);
		int blank_room;
		remove_node(target);
		remove_node(target);
		if(room_req()<0){
			return -1;
		}
		if((blank_room=room_resp(room_key))<0){
			return -1;
		}
		*player[0].room_num = blank_room;
		*player[1].room_num = blank_room;
		strcpy(player[0].room_key,room_key);
		strcpy(player[1].room_key,room_key); 
		*player[0].room_id = 0;
		*player[1].room_id = 1;
        sem_post(player[0].user_sem);
		sem_post(player[1].user_sem);
		return 1;
	}
	return 0;
}

int room_resp(char *room_key){
	if(version_check(room_serv_sock)<0){
        return -1;
    }
    switch(msg_type_check(room_serv_sock)){
        case 2:
            break;
        default:
            return -1;
    }
    unsigned char msg[BUFFER_SIZE];
    memset(msg,0,sizeof(msg));
    char msg_length;
    int buf_len,used_len=0;
    int blank_room;
    if(recv(room_serv_sock,&msg_length,1,0)<=0){
        return -1;
    }
    while(1){
        buf_len = recv(room_serv_sock,msg + used_len,msg_length,0);
        if(buf_len<=0){
            return -1;
        }else if(!(msg_length -= buf_len)){
            used_len += buf_len;
            break;
        }else{
            used_len += buf_len;
        }
    }
    sscanf(msg,"%d %s",&blank_room,room_key);
    return blank_room;
}

int room_req(void){
	unsigned char req[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	int length;
	memset(req,0,sizeof(req));
    memset(buffer,0,sizeof(buffer));
    snprintf(buffer,BUFFER_SIZE,"%d",P_NUM);
    req[0] = VERSION_1;
    req[1] = VERSION_2; //version:0.1
    req[2] = 1; //msg_type:room request
    req[3] = (char)strlen(buffer) + 1; //sizeof payload
    req[4] = '\0';
    strncat(&req[4],buffer,strlen(buffer)+1);
    length = strlen(buffer)+5;
	if(send(room_serv_sock,req,length,0)<0){
        printf("message send error\n");
        return -1;
    }
	return 0;
}

void Width_Update(node_t * p,time_t now) {
	time_t Wating_Time = now - p->data.start_time;
	if (Wating_Time < 30) {
		return;
	}
	else if (Wating_Time < 60) {
		p->data.width = SECOND_WAITING_WIDTH;
		return;
	}
	else if (Wating_Time < 120) {
		p->data.width = THIRD_WAITING_WIDTH;
		return;
	}
	else {
		return;
	}
}