#include <stdio.h>
#include <stdlib.h>

#include "usefulFuncs.h"
#include "dialog.h"
#include "Btree.h"

/**
 * @mainpage Консольное приложение для хранения хеш-таблицы в бинарном файле
 * Cостоит из следующих частей:
 * - @ref Dialog
 * - @ref Tree
 */

int main(){

	const char *msgs[] = {	"Commands:",
							"\t0. Exit",
							"\t1. New",
							"\t2. Delete", 
							"\t3. Find", 
							"\t4. Special find",
							"\t5. Print",
							"\t6. Special print",
							"\t7. Import",
							"\t8. Start benchmark",
							"\t9. Commands"};
	const int nMsgs = sizeof(msgs) / sizeof(msgs[0]);

	int (*fptr[])(Tree *tree) = {NULL, dAdd, dDelete, dFind, dSmartFind, dPrint, dSpecialPrint};

	Tree *tree = createTree();

	int rc, errorCode = 0;

	for(int i = 0; i < nMsgs; i++)
		printf("%s\n", msgs[i]);

	while(1){
		rc = dialog(msgs, nMsgs);
		if (rc == -1){
			printf("No such command!\n");
			continue;
		}
		else if (rc == 0)
			break;
		else if (0 < rc && rc < 7){
			errorCode = fptr[rc](tree);
			if (!errorCode){
				printf("Success!\n");
				if (rc == 7)
					printf("To make it work, enter this command [make gen] in the same directory where you are now.\n");
			}
			else
				printf("Error!\n");
			errorCode = 0;
		}
		else if (rc == 7) {
			errorCode = importData(&tree);
			if (errorCode)
				printf("File related error!\n");
			else
				printf("Success!\n");
			errorCode = 0;
		}
		else if (rc == 8){
			int errorCode = dBenchmark();
			if (errorCode)
				printf("Failed!\n");
			else
				printf("Success!\n");
		}
		else if (rc == 9) {
			for(int i = 0; i < nMsgs; i++)
				printf("%s\n", msgs[i]);
		}
	}

	deleteTree(tree);

	return 0;
}
