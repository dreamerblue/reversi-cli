#include "main.h"

// Cell's max width equals the length of 3 special chars
char box_drawing_char[][special_char_size*3] = {
		"┌", "┬", "┐",
		"├", "┼", "┤",
		"└", "┴", "┘",
		"───", "│",
	};

// Grid lines' row/column number equals board_size*2+1
char board[board_size*2+1][board_size*2+1][special_char_size*4];

char highlight_begin[] = "\033[7m";
char highlight_end[] = "\033[0m";

char black_label[] = "●";
char white_label[] = "○";

void InitializeGridLines() {
	for(int i=0; i<=board_size*2; i+=2) {
		if(i == 0) {
			strcpy(board[i][0], box_drawing_char[0]);
			for(int j=2; j<board_size*2; j+=2) {
				strcpy(board[i][j], box_drawing_char[1]);
			}
			strcpy(board[i][board_size*2], box_drawing_char[2]);
		}
		else if(i == board_size*2) {
			strcpy(board[i][0], box_drawing_char[6]);
			for(int j=2; j<board_size*2; j+=2) {
				strcpy(board[i][j], box_drawing_char[7]);
			}
			strcpy(board[i][board_size*2], box_drawing_char[8]);
		}
		else {
			strcpy(board[i][0], box_drawing_char[3]);
			for(int j=2; j<board_size*2; j+=2) {
				strcpy(board[i][j], box_drawing_char[4]);
			}
			strcpy(board[i][board_size*2], box_drawing_char[5]);
		}
		for(int j=1; j<board_size*2; j+=2) {
			strcpy(board[i][j], box_drawing_char[9]);
		}
	}
	for(int i=1; i<board_size*2; i+=2) {
		for(int j=0; j<=board_size*2; j+=2) {
			strcpy(board[i][j], box_drawing_char[10]);
		}
	}
}

void InitializeBoard() {
	InitializeGridLines();
	pieces[board_size/2-1][board_size/2].color = BLACK;
	pieces[board_size/2][board_size/2-1].color = BLACK;
	pieces[board_size/2-1][board_size/2-1].color = WHITE;
	pieces[board_size/2][board_size/2].color = WHITE;
	UpdatePieces();
}

// Update pieces' data fow drawing
void UpdatePieces() {
	for(int i=1; i<board_size*2; i+=2) {
		for(int j=1; j<board_size*2; j+=2) {
			strcpy(board[i][j], pieces[i/2][j/2].GetContent());
		}
	}
}

void ShowCheckedPoint(bool show) {
	if(show) checked = checked_bak;
	else {
		checked_bak = checked;
		checked.x = checked.y = -1;
	}
}

void DrawBoard() {
	// Clear screen
	printf("\033[2J");
	printf("\033[0;0H");
	for(int i=0; i<=board_size*2; ++i) {
		for(int j=0; j<=board_size*2; ++j) {
			// Add highlight for checked point
			if(i==checked.x*2+1 && j==checked.y*2+1)
				printf("%s", highlight_begin);
			// Draw board
			printf("%s", board[i][j]);
			// End highlight
			if(i==checked.x*2+1 && j==checked.y*2+1)
				printf("%s", highlight_end);
		}
		// Show time
		if(i == 4) {
			printf("%8sTime: ", "");
			if(time_used > 60*60) {
				printf("%d:", time_used/(60*60));
				time_used %= 60*60;
				printf("%02d:", time_used/60);
			}
			else printf("%d:", time_used/60);
			printf("%02d", time_used%60);
		}
		// Show turn
		if(show_turn && i==7)
			printf("%8sTurn: %s", "", turn==BLACK? black_label : white_label);
		// Show turn (skip)
		if(show_turn_skip && i==7)
			printf("%8s\033[31m%s No position, skip\033[0m",
				"", turn==BLACK? black_label : white_label);
		// Show time
		if(show_result && i==7) {
			if(black_num == white_num)
				printf("%8sA draw", "");
			if(black_num > white_num)
				printf("%8sWinner: %s", "", black_label);
			if(white_num > black_num)
				printf("%8sWinner: %s", "", white_label);
		}
		// Show number(Black)
		if(i == 9) printf("%8s%s %d", "", black_label, black_num);
		// Show number(White)
		if(i == 10) printf("%8s%s %d", "", white_label, white_num);
		printf("\n");
	}
}