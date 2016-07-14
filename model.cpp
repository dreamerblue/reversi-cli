#include "main.h"

const int direction_num = 8;
point checked = {-1, -1};
point checked_bak = {0, 0};
grid pieces[board_size][board_size];
int turn = BLACK;
int black_num = 2;
int white_num = 2;
int direction[direction_num][2] = {
		{-1, 0}, {-1, 1}, {0, 1}, {1, 1},
		{1, 0}, {1, -1}, {0, -1}, {-1, -1},
	};
bool show_turn = false;
bool show_turn_skip = false;
bool show_result = false;
int time_used = 0;
itimerval value;
timeval start_time, end_time;

// Sleep function (auto-restart if interrupted by an alarm signal)
void Sleep(int us) {
	timespec ts, ts_r;
	ts.tv_sec = us / USECS_PER_SEC;
	ts.tv_nsec = us % USECS_PER_SEC * NSECS_PER_USEC;
	while (nanosleep(&ts, &ts_r) == -1) {
		ts = ts_r;
	}
}

bool InBoard(point p) {
	if(p.x>=0 && p.x<board_size
		&& p.y>=0 && p.y<board_size)
		return true;
	else return false;
}

bool CanBeReversed(point p) {
	if(InBoard(p) && -1*pieces[p.x][p.y].color==turn)
		return true;
	else return false;
}

// Generate all next positions in this turn
// If no position, return false
bool GenerateNextPositions() {
	point next;
	bool has_position = false;
	for(int i=0; i<board_size; ++i) {
		for(int j=0; j<board_size; ++j) {
			// Check all empty positions
			if(pieces[i][j].color == NONE) {
				// Traverse all 8 directions
				for(int k=0; k<direction_num; ++k) {
					bool has_reversed = false;
					next.x = i + direction[k][0];
					next.y = j + direction[k][1];
					while(CanBeReversed(next)) {
						has_reversed = true;
						next.x = next.x + direction[k][0];
						next.y = next.y + direction[k][1];
					}
					if(has_reversed && InBoard(next)
						&& pieces[next.x][next.y].color==turn) {
						pieces[i][j].legal = true;
						has_position = true;
						break;
					}
				}
			}
		}
	}
	UpdatePieces();

	return has_position;
}

// Judge and reverse piece(s)
void Judge(point p) {
	ShowCheckedPoint(false);
	// "reversed" stores piece(s) can be reversed
	vector<point> reversed;
	point next;
	pieces[p.x][p.y].color = turn;
	int num = 0;
	//Clear all legal tag
	for(int i=0; i<board_size; ++i) {
		for(int j=0; j<board_size; ++j) {
			pieces[i][j].legal = false;
		}
	}
	UpdatePieces();
	DrawBoard();
	Sleep(500000);
	// Traverse all 8 directions
	for(int i=0; i<direction_num; ++i) {
		next.x = p.x + direction[i][0];
		next.y = p.y + direction[i][1];
		// "tmp" temporarily stores the opposite piece(s)
		vector<point> tmp;
		while(CanBeReversed(next)) {
			tmp.push_back(next);
			next.x = next.x + direction[i][0];
			next.y = next.y + direction[i][1];
		}
		// If piece(s) in "tmp" can be reversed, copy to "reversed"
		if(InBoard(next) && pieces[next.x][next.y].color==turn) {
			num += tmp.size();
			for(int i=0; i<tmp.size(); ++i) {
				reversed.push_back(tmp[i]);
			}
		}
	}
	// Draw piece(s) in "reversed" as "|" (Only for animation)
	for(int i=0; i<reversed.size(); ++i) {
		pieces[reversed[i].x][reversed[i].y].being_reversed = true;
	}
	UpdatePieces();
	DrawBoard();
	Sleep(500000);
	// Update number
	if(turn == BLACK) {
		black_num += 1 + num;
		white_num -= num;
	}
	if(turn == WHITE) {
		white_num += 1 + num;
		black_num -= num;
	}
	// Reverse color
	for(int i=0; i<reversed.size(); ++i) {
		pieces[reversed[i].x][reversed[i].y].ReverseColor();
	}
	UpdatePieces();
	DrawBoard();
	Sleep(1000000);
	turn *= -1;
}

// Set timer for refreshing time
void SetTimer(int interval_us) {
	value.it_value.tv_sec = interval_us / USECS_PER_SEC;
	value.it_value.tv_usec = interval_us % USECS_PER_SEC;
	value.it_interval.tv_sec = interval_us / USECS_PER_SEC;
	value.it_interval.tv_usec = interval_us % USECS_PER_SEC;
	setitimer(ITIMER_REAL, &value, NULL);
}

// Refresh time if catches SIGALRM signal
void RefreshTimeViaTimer(int signal_no) {
	gettimeofday(&end_time, NULL);
	time_used = end_time.tv_sec - start_time.tv_sec;
	DrawBoard();
}

// Set terminal
void SetTerm(int type) {
	if(type == 0) {
		system("stty echo icanon");
		printf("\e[?25h");	//show cursor
	}
	else {
		system("stty -echo -icanon");
		printf("\e[?25l");	//hide cursor
	}
}

void InitializeSettings() {
	signal(SIGINT, Quit);
	signal(SIGALRM, RefreshTimeViaTimer);
	SetTimer(1000000);	// refresh time every second
	SetTerm(1);
	gettimeofday(&start_time, NULL);	// set start time
}

void PVP() {
	InitializeSettings();
	int key_hit;
	bool last_turn_has_next;
	while(true) {
		ShowCheckedPoint(true);
		if(GenerateNextPositions()) {
			show_turn = true;
			show_turn_skip = false;
			last_turn_has_next = true;
			DrawBoard();
			// Get single key
			while((key_hit = GetKey())) {
				point next = checked;
				if(key_hit == ENTER) {
					if(pieces[checked.x][checked.y].legal) {
						Judge(checked);
						break;
					}
				}
				else if(key_hit==UP || key_hit=='W' || key_hit=='w')
					next.x--;
				else if(key_hit==LEFT || key_hit=='A' || key_hit=='a')
					next.y--;
				else if(key_hit==DOWN || key_hit=='S' || key_hit=='s')
					next.x++;
				else if(key_hit==RIGHT || key_hit=='D' || key_hit=='d')
					next.y++;
				else if(key_hit == ESC) Quit(0);
				else continue;
				if(InBoard(next)) {
					checked = next;
					DrawBoard();
				}
			}
		}
		else {
			// If both players have no position for next, break
			if(!last_turn_has_next) break;
			// If there's no position, break
			if(black_num+white_num == board_size*board_size) break;
			ShowCheckedPoint(false);
			show_turn = false;
			show_turn_skip = true;
			last_turn_has_next = false;
			DrawBoard();
			Sleep(2000000);
			turn *= -1;
		}
	}
	//End game
	SetTimer(0);
	ShowCheckedPoint(false);
	show_turn = false;
	show_turn_skip = false;
	show_result = true;
	DrawBoard();
	SetTerm(0);
}