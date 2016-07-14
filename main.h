#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <vector>
#include <unistd.h>
#include <sys/time.h>
#define BLACK -1
#define NONE 0
#define WHITE 1
#define ENTER 10
#define ESC 27
#define UP 183
#define DOWN 184
#define RIGHT 185
#define LEFT 186
#define USECS_PER_SEC 1000000
#define NSECS_PER_USEC 1000
using namespace std;

const int board_size = 8;
const int special_char_size = 5;


//Model

struct point {
	int x;
	int y;
};

struct grid {
	int color;
	char content[special_char_size*4]; // color/status string
	bool checked;			// true: being checked by user
	bool legal;				// true: legal position for next step
	bool being_reversed;	// true: being reversed now
	void ReverseColor() {
		being_reversed = false;
		color *= -1;
	}
	char * GetContent() {
		if(color == BLACK) strcpy(content, " ● ");
		if(color == NONE) strcpy(content, "   ");
		if(color == WHITE) strcpy(content, " ○ ");
		if(legal) strcpy(content, "\033[36m x \033[0m");
		if(being_reversed) strcpy(content, " | ");

		return content;
	}
};

extern point checked;
extern point checked_bak;
extern grid pieces[board_size][board_size];
extern int turn;
extern int black_num;
extern int white_num;
extern int time_used;
extern bool show_turn;
extern bool show_turn_skip;
extern bool show_result;

void SetTerm(int type);
void PVP();


//View

void InitializeBoard();
void UpdatePieces();
void ShowCheckedPoint(bool);
void DrawBoard();


//Controller

int GetKey();
void Quit(int);