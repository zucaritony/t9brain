#include <string.h>
#include <stdlib.h>

void make_letters(char** letters) {
	letters[0] = (char*)malloc(1*sizeof(char));
	letters[1] = (char*)malloc(1*sizeof(char));
	letters[2] = (char*)malloc(3*sizeof(char));
	letters[3] = (char*)malloc(3*sizeof(char));
	letters[4] = (char*)malloc(3*sizeof(char));
	letters[5] = (char*)malloc(3*sizeof(char));
	letters[6] = (char*)malloc(3*sizeof(char));
	letters[7] = (char*)malloc(4*sizeof(char));
	letters[8] = (char*)malloc(3*sizeof(char));
	letters[9] = (char*)malloc(4*sizeof(char));
	
	strcpy(letters[0], " ");
	strcpy(letters[1], "'");
	strcpy(letters[2], "abc");
	strcpy(letters[3], "def");
	strcpy(letters[4], "ghi");
	strcpy(letters[5], "jkl");
	strcpy(letters[6], "mno");
	strcpy(letters[7], "pqrs");
	strcpy(letters[8], "tuv");
	strcpy(letters[9], "wxyz");
}
