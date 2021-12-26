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
#include<netinet/in.h>
#include<arpa/inet.h>
WINDOW * message_win;

void draw_paddle(WINDOW *win, paddle_position_t * paddle, int delete, bool player){
    int ch;
    if(delete){
        if(player) ch = '=';
        else ch = '_';
    }else{
        ch = ' ';
    }   
    int start_x = paddle->x - paddle->length;
    int end_x = paddle->x + paddle->length;
    for (int x = start_x; x <= end_x; x++){
        wmove(win, paddle->y, x);
        waddch(win,ch);
    }
    wrefresh(win);
}
void draw_ball(WINDOW *win, ball_position_t * ball, int draw){
    int ch;
    if(draw){
        ch = ball->c;
    }else{
        ch = ' ';
    }
    wmove(win, ball->y, ball->x);
    waddch(win,ch);
    wrefresh(win);
}
void drawboard(WINDOW *win, server_message s, bool draw){
    bool player = false;
    paddle_position_t paddle;
    paddle.length=PADLE_SIZE;
    ball_position_t ball;
    ball.y = s.ball.y;
    ball.x = s.ball.x;
    ball.c='o';
    draw_ball(win, &ball, draw);
    
    for(int i=0;i<MAX_PLAYERS;i++){
        if(s.player_id==i) player = true;
        else player = false;
        if(s.p[i].x<0) continue;
        paddle.y = s.p[i].y;
        paddle.x = s.p[i].x;

        draw_paddle(win, &paddle, draw,player);
    }
    
}

void scoreboard(WINDOW *win, server_message s){
    
    int count=0;
    for(int i=0;i<MAX_PLAYERS;i++) if(s.p[i].x>0) count++;

    message_win = newwin(2+count*2, WINDOW_SIZE+10, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);  

    for(int i=0;i<MAX_PLAYERS;i++){
        if(i==s.player_id){
           mvwprintw(message_win, 1+i*2,1,"P%i score %i <---",i+1,s.p[i].score);  
        } 
        else if((s.p[i].x>0))mvwprintw(message_win, 1+i*2,1,"P%i score %i",i+1,s.p[i].score);
    }
    wrefresh(message_win);
    return;
}



int main(int argc, char *argv[])
{
    
    int sock_fd= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1){
        perror("socket: ");
        exit(-1);
    }
    if(argc<2){
        printf("No Adress provided\n");
        exit(-1);
    }
    char* str=argv[1];
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);

    if( inet_pton(AF_INET, str, &server_addr.sin_addr) < 1){
        printf("no valid address: \n");
        exit(-1);
    }

    player_message m;
    server_message s;
    m.player_id=-1;
    m.move = 0;

    int t =sendto(sock_fd,&m, sizeof(m), 0,(const struct sockaddr *) &server_addr, sizeof(server_addr));
    t = recv(sock_fd, &s, sizeof(s), 0);
    
    if (s.player_id==-1){
        printf("Too many players, can't connect\n");
        exit(1);
    }
    m.player_id=s.player_id;

	initscr();			/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0); 
    wrefresh(my_win);
    keypad(my_win, true);
    /* creates a window and draws a border */
  
  
    drawboard(my_win,s,true);
    scoreboard(message_win,s);
    
    int key = -1;
    while(key != 'q'){
        key = wgetch(my_win);       
        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN || key == 'q'){
            m.move=key;
            t =sendto(sock_fd,&m, sizeof(m), 0,(const struct sockaddr *) &server_addr, sizeof(server_addr));
            drawboard(my_win,s,false);
            t = recv(sock_fd, &s, sizeof(s), 0);
            drawboard(my_win,s,true);
            scoreboard(message_win,s);
            
            
              
        }
    }
    
    
  	endwin();			/* End curses mode		  */
    printf("player disconected\n");
    close(sock_fd);
	return 0;
}