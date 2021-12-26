#ifndef SUPER
#define SUPER
#define SOCK_PORT 5000
#define WINDOW_SIZE 20
#define PADLE_SIZE 2
#define SOCK_ADDRESS "/tmp/sock_16"
#define MAX_PLAYERS 10
#
typedef struct player_message
{
    int player_id;
    int move;
}player_message;

typedef struct ppos
{
	int x;
	int y;
	int score;
}ppos;

typedef struct ball
{
	int x;
 	int y;
}balls;

typedef struct server_message
{
	int player_id;
    balls ball;
    ppos p[MAX_PLAYERS];
}server_message;

typedef struct paddle_position_t{
    int x, y;
    int length;
} paddle_position_t;

typedef struct ball_position_t{
    int x, y;
    int up_hor_down; //  -1 up, 0 horizontal, 1 down
    int left_ver_right; //  -1 left, 0 vertical,1 right
    char c;
} ball_position_t;
#endif// TODO_1