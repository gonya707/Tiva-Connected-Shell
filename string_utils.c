#include "string_utils.h"
#include "string.h"

int isChar(char c){
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '\r')
			|| (c == ' ')// || (c == 127)
	);
}

void stringReset(char* s){
	/* Y U NO WORK
	int k;
	for (k = 0; k < MAX_LENGTH; k++){
		s[k] = ' ';
	}*/

	strcpy(s, "                                                                               ");
}
