#include <stdio.h>
#include <limits.h>

#include "usefulFuncs.h"

int inputInt(int *n){
	int errorCode = 0;

	int code;
	char c[81];

	do{
		code = scanf("%d", n);
		if (code == -1) 
			return 1;
		if (code == 0){
			printf("Incorrect input data!\n");
			scanf("%*c", c);
			continue;
		}

	} while(code != 1);

	return 0;
}

int inputUnsigned(unsigned int *n){
	char trash[80];
	int code;
	long long int tmp;
	do{
		code = scanf("%lld", &tmp);
		if (code == -1)
			return 1;
		if (code == 0 || tmp < 0 || tmp > UINT_MAX){
			printf("Incorrect input data!\n");
			scanf("%*c", trash);
			continue;
		}
		else{
			(*n) = (unsigned int) tmp;
			break;
		}
	} while (1);


	return 0;
}

int dialog(const char *msgs[], const int nMsgs){
	int rc = -1;
	int code = 0;
	printf("> ");
	code = inputInt(&rc);
	if (code == 1)
		return 0;
	for(int i = 0; i < nMsgs; i++){
		if (i == rc)
			return i;
	}

	return rc;
}