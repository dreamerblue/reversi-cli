#include "main.h"

int GetKey() {
	char buf[3];
	int key = 0;
	int num = read(0, buf, sizeof(buf));
	for(int i=0; i<num; ++i) {
		key += buf[i];
	}

	return key;
}

// if quit, set terminal as original settings
void Quit(int signal_no) {
	SetTerm(0);
	exit(signal_no);
}