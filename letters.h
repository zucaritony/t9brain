#include <string.h>
#include <stdlib.h>


	char* letters[10];
	letters[2] = malloc(3*sizeof(char));
	letters[3] = malloc(3*sizeof(char));
	letters[4] = malloc(3*sizeof(char));
	letters[5] = malloc(3*sizeof(char));
	letters[6] = malloc(3*sizeof(char));
	letters[7] = malloc(4*sizeof(char));
	letters[8] = malloc(3*sizeof(char));
	letters[9] = malloc(4*sizeof(char));
	
	strcpy(letters[2], "abc");
	strcpy(letters[3], "def");
	strcpy(letters[4], "ghi");
	strcpy(letters[5], "jkl");
	strcpy(letters[6], "mno");
	strcpy(letters[7], "pqrs");
	strcpy(letters[8], "tuv");
	strcpy(letters[9], "wxyz");
	
