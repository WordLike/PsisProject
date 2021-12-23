#include <ncurses.h>
#include "super_pong.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
 #include <ctype.h> 
 #include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>



void new_paddle (paddle_position_t * paddle, int legth){
    paddle->x = rand() % (WINDOW_SIZE);
    paddle->y = rand() % (WINDOW_SIZE);
    paddle->length = legth;
}

void moove_paddle (paddle_position_t * paddle, int direction){
    
    if (direction == KEY_UP){

        if (paddle->y  != 1){
            
            paddle->y --;
            
        }

    }
    if (direction == KEY_DOWN){
        if (paddle->y  != WINDOW_SIZE-2){
            paddle->y ++;
        }
    }
    

    if (direction == KEY_LEFT){
        if (paddle->x - paddle->length != 1){
            paddle->x --;
        }
    }
    if (direction == KEY_RIGHT)
        if (paddle->x + paddle->length != WINDOW_SIZE-2){
            paddle->x ++;
    }
}

void place_ball_random(ball_position_t * ball){
    ball->x = rand() % (WINDOW_SIZE-1)+1 ;
    ball->y = rand() % (WINDOW_SIZE-1)+1 ;
    ball->c = 'o';
    ball->up_hor_down = rand() % 3 -1; //  -1 up, 1 - down
    ball->left_ver_right = rand() % 3 -1 ; // 0 vertical, -1 left, 1 right
}
void moove_ball(ball_position_t * ball,ppos *p,int length){
    
    int next_x = ball->x + ball->left_ver_right;
    int next_y = ball->y + ball->up_hor_down;
    for(int i=0;i<MAX_PLAYERS;i++){
        if((p[i].x+length)>=next_x &&(p[i].x-length)<=next_x && p[i].y==next_y){
            p[i].score++;
            if(p[i].y!=ball->y){

                ball->up_hor_down *= -1;
                ball->left_ver_right = rand() % 3 -1;
                ball->x = next_x;               
            }
            else{

                ball->up_hor_down = rand() % 3 -1 ;
                ball->left_ver_right *= -1;
                ball -> y = next_y;

            }  
            return;
        }
    }
    if( next_x == 0 || next_x == WINDOW_SIZE-1){
        ball->up_hor_down = rand() % 3 -1 ;
        ball->left_ver_right *= -1;
     }else{
        ball->x = next_x;
    }
    next_y = ball->y + ball->up_hor_down;
    if( next_y == 0 || next_y == WINDOW_SIZE-1){
        ball->up_hor_down *= -1;
        ball->left_ver_right = rand() % 3 -1;
    }else{
        ball -> y = next_y;
    }
}

int select_free_player(ppos* p){
    
    for(int i=0; i<MAX_PLAYERS;i++){
        if(p[i].x==-1){
            return i;
        }
    }
    return -1;
}

void init(ppos **p, player_message *m, server_message *s){
    //inicializa o que for preciso
    m->player_id = -1;
    m->move = 0;

    s-> ball.x = 0;
    s-> ball.y = 0;
    *p=s-> p;
    for(int i=0; i<MAX_PLAYERS;i++){
        (*p)[i].score = 0;
        (*p)[i].x = -1;
        (*p)[i].y = -1;
    }
    
}

bool valid_move(int length, ppos *p, int player_id, paddle_position_t paddle){

    if(paddle.y<=0||paddle.y>=WINDOW_SIZE-1||paddle.x-length<=0||paddle.x+length>=WINDOW_SIZE-1){
        return 0;
    }
    
    for(int i=0; i<MAX_PLAYERS;i++){
        if(i!=player_id && p[i].x!=-1){
            if(p[i].y!=paddle.y) continue;
            if((p[i].x+length)<(paddle.x-length)||(p[i].x-length)>(paddle.x+length)) continue;
            printf("%i",player_id);
            return 0;     
        }
    }
    return 1;
}

paddle_position_t paddle;
ball_position_t ball;

int main(){
    srand(time(0));
    ppos *p;//players in the game
    int t;//receive returns from functions with useless returns
    player_message m;
    server_message s;
    paddle_position_t paddle;
    int player_num=0;

    init(&p,&m,&s);

    int sock_fd;
    struct sockaddr_un local_addr;
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path, SOCK_ADDRESS);
    sock_fd= socket(AF_UNIX, SOCK_DGRAM, 0);
    unlink(SOCK_ADDRESS);
    struct sockaddr_un client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_un);

    if (sock_fd == -1){
        perror("socket: ");
        exit(-1);
    }
    int err = bind(sock_fd, (struct sockaddr *)&local_addr,
                   sizeof(local_addr));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }

    place_ball_random(&ball);
    s.ball.x = ball.x;
    s.ball.y = ball.y;
    int n=1;
    
    do
    {
        
        t = recvfrom(sock_fd, &m, sizeof(m), 0, ( struct sockaddr *)&client_addr, &client_addr_size);

        if(m.player_id==-1)//is a connect message
        {
            s.player_id=select_free_player(p);
            printf("%i",s.player_id);
            if (s.player_id>=0){
                do{
                    new_paddle(&paddle, PADLE_SIZE);
                    
                }
                while(!valid_move(PADLE_SIZE,p,s.player_id,paddle));
                player_num++;
                p[s.player_id].x=paddle.x;
                p[s.player_id].y=paddle.y;
                
            }
        }
        else{
            if(m.move=='q'){//disconnect
                p[s.player_id].x=-1;
                p[s.player_id].y=-1;
                p[s.player_id].score=0;
                s.player_id=-1;
                player_num--;
            }
            else{
                s.player_id=m.player_id;
                paddle.x=p[s.player_id].x;
                paddle.y=p[s.player_id].y;
                
                moove_paddle(&paddle, m.move);

                if(valid_move(PADLE_SIZE,p,s.player_id,paddle)){
                    p[s.player_id].x=paddle.x;
                    p[s.player_id].y=paddle.y;
                    
                }
                if(n<player_num){
                    n++;
                }
                else{
                    n=1;
                    moove_ball(&ball,p,PADLE_SIZE);
                }
                s.ball.x = ball.x;
                s.ball.y = ball.y;
            }
        }
        t =sendto(sock_fd,&s, sizeof(s), 0,(const struct sockaddr *) &client_addr, client_addr_size);
      
    }while(1);
    printf("Server Ended Correctly\n");
    
             /* End curses mode        */
    close(sock_fd);
    return 0;
}
