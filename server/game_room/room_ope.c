#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "room_server.h"

#define BIT(n) (1 << n)
#define G_FLAG BIT(0)
#define M_FLAG BIT(1)

static int server_init(int port);
static int game_from_check(struct sockaddr_in* game_req_from);
static int match_from_check(struct sockaddr_in* match_req_from);

int main(void){
    memset(p2_used_room,0,sizeof(p2_used_room));
    int game_sock,match_sock;
    int game_req_sock;
    int match_req_sock;
    struct sockaddr_in game_req_from;
    struct sockaddr_in match_req_from;
    int g_from_len = sizeof(game_req_from);
    int m_from_len = sizeof(match_req_from);
    char flag = 0;
    fd_set read_fds;

    if((game_sock=server_init(TO_GAMESERV_PORT))<0){
        return -1;
    }else if((match_sock=server_init(TO_MATCHSERV_PORT))<0){
        close(game_sock);
        return -1;
    }
    int width = (game_sock > match_sock) ? game_sock+1 : match_sock+1;
    
    while(1){
        FD_ZERO(&read_fds);
        FD_SET(game_sock,&read_fds);
        FD_SET(match_sock,&read_fds);
        if(flag & G_FLAG){
            FD_SET(game_req_sock,&read_fds);
            if(game_req_sock + 1 > width){
                width = game_req_sock + 1;
            }
        }
        if(flag & M_FLAG){
            FD_SET(match_req_sock,&read_fds);
            if(match_req_sock + 1 > width){
                width = match_req_sock + 1;
            }
        }
        if(select(width,&read_fds,NULL,NULL,NULL) <= 0){
            if(errno == EINTR) continue;
            goto error;
        }
        if(FD_ISSET(game_sock,&read_fds)){
            if(flag & G_FLAG) continue;
            game_req_sock = accept(game_sock,(struct sockaddr *)&game_req_from,&g_from_len);
            if(game_from_check(&game_req_from)<0){
                close(game_req_sock);
                continue;
            }
            fprintf(stderr,"game server connected...\n");
            flag |= G_FLAG;
        }else if(FD_ISSET(match_sock,&read_fds)){
            if(flag & M_FLAG) continue;
            match_req_sock = accept(match_sock,(struct sockaddr *)&match_req_from,&m_from_len);
            if(match_from_check(&match_req_from)<0){
                close(match_req_sock);
                continue;
            }
            fprintf(stderr,"matching server connected...\n");
            flag |= M_FLAG;
        }
        if(flag & G_FLAG){
            if(FD_ISSET(game_req_sock,&read_fds)){
                if(game_msg_parse(game_req_sock)<0){
                    goto error;
                }
            }
        }    
        if(flag & M_FLAG){
             if(FD_ISSET(match_req_sock,&read_fds)){
                if(match_msg_parse(match_req_sock)<0){
                    goto error;
                }
            }
        }    
    }
    error:
        close(game_sock);
        close(match_sock);
        if(flag & G_FLAG){
            close(game_req_sock);
        }
        if(flag & M_FLAG){
            close(match_req_sock);
        }

    return 0;
}

int server_init(int port){
    //error check
    int result;
    //setsockopt
    char opt=1;
    //listen
    struct sockaddr_in srcAddr;
    int srcSocket;
    //srcaddr init
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_port = htons(port);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* socket */
    srcSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(srcSocket == -1){
        printf("socket error\n");
        return -1;
    }
    if(setsockopt(srcSocket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int))!=0){
        printf("setsockopt error\n");
        close(srcSocket);
        return -1;
    }
    /* bind */
    result = bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));
    if(result == -1){
        printf("bind error\n");
        close(srcSocket);
        return -1;
    }
    /* listen */
    result = listen(srcSocket, SOMAXCONN);
    if(result == -1){
        printf("listen error\n");
        close(srcSocket);
        return -1;
    }
    return srcSocket;
}

int game_from_check(struct sockaddr_in* game_req_from){
    if(strcmp(inet_ntoa(game_req_from->sin_addr),GAME_SERV_ADDR)){
        return -1;
    }else if(ntohs(game_req_from->sin_port) != GAME_SERV_PORT){
        return -1;
    }
    return 0;
}

int match_from_check(struct sockaddr_in* match_req_from){
    if(strcmp(inet_ntoa(match_req_from->sin_addr),MATCH_SERV_ADDR)){
        return -1;
    }else if(ntohs(match_req_from->sin_port) != MATCH_SERV_PORT){
        return -1;
    }
    return 0;
}

int p2_room_search(void){
    int i;
    for(i=0;i<P2_MAX_ROOM;++i){
        if(p2_used_room[i]==0){
            p2_used_room[i] = 1;
            return i;
        } 
    }
    return -1;
}

void p2_room_return(int return_room){
    p2_used_room[return_room] = 0;
}