#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "letters.c"

char getcc();

int main(int argc, char** argv) {	
	char* letters[10];
	make_letters(letters);
	
	
	return 0;
}

char getcc() {
	char grab;
	int error = read(STDIN_FILENO, &grab, 1);
	
	if(error != -1)
		return grab;
	else
		return -1;
}
