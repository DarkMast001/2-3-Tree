#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "usefulFuncs.h"
#include "dialog.h"
#include "Btree.h"

/**
 * @page Dialog Диалоговые функции меню
 */

int dAdd(Tree *tree){
    unsigned int key;
    char* info;

    int errorCode = 0;

    printf("Input key (integer): ");
    errorCode = inputUnsigned(&key);
    if (errorCode)
        return 1;

    info = readline("Input value (string): ");
    if (info == NULL)
        return 1;

    errorCode = addNode(tree, key, info);
    if (errorCode)
        return 1;

    // free(info);

	return 0;
}

int dDelete(Tree *tree){
    unsigned int key;
    int errorCode = 0;

    printf("Enter the key to be deleted: ");
    errorCode = inputUnsigned(&key);
    if (errorCode)
        return 1;

    return deleteNode(tree, key, 0);
}

int dFind(Tree *tree){
    int key;
    int errorCode = 0;

    printf("Enter the key to be found: ");
    errorCode = inputUnsigned(&key);
    if (errorCode)
        return 1;

    Info *info = findInfoByKey(tree, key);
    if (info == NULL){
        printf("No elements whith this key!\n");
        return 1;
    }
    if (info->next == NULL){
        printf("Key: %u - %s\n", key, info->str);
        return 0;
    }

    int serial;
    printf("There are several values under this key, enter the serial number of the information: ");
    errorCode = inputInt(&serial);
    if (errorCode)
        return 1;
    for(int i = 1; info != NULL; i++){
        if (i == serial){
            printf("Key: %u - %s\n", key, info->str);
            return 0;
        }
        info = info->next;
    }

    return 1;
}

int dSmartFind(Tree *tree){
    int keyLowerLimit;
    int errorCode = 0;

    printf("Enter the key - lower limit above which you want to find: ");
    errorCode = inputInt(&keyLowerLimit);
    if (errorCode)
        return 1;

    Info *info = smartFindNodeByKey(tree, keyLowerLimit);
    if (info == NULL){
        printf("No elements whith this key!\n");
        return 1;
    }
    if (info->next == NULL){
        printf("Key: %u - %s\n", info->key, info->str);
        return 0;
    }

    int serial;
    printf("There are several values under this key, enter the serial number of the information: ");
    errorCode = inputInt(&serial);
    for(int i = 1; info != NULL; i++){
        if (i == serial){
            printf("Key: %u - %s\n", info->key, info->str);
            return 0;
        }
        info = info->next;
    }

    return 1;

    return 0;
}

int dPrint(Tree *tree){
    int errorCode = printTree(tree);
    if (errorCode){
        printf("Tree is empty!\n");
        return 1;
    }

    return 0;
}

int dSpecialPrint(Tree *tree){
    int errorCode = printReverseKeyOrder(tree);
    if (errorCode){
        printf("Tree is empty!\n");
        return 1;
    }

    return 0;
}

int dBenchmark(){
    int numberOfTestsPerIteration = 10;  // 7
    int numberOfIterations = 10; // 7
    int valueIncrement = 5000;  // 5000
    int initialValue = 5000;    // 5000

    printf("Default parameters:\n");
    printf("\tNumber of tests per iteration: %d\n", numberOfTestsPerIteration);
    printf("\tNumber of iterations: %d\n", numberOfIterations);
    printf("\tValue increment: %d\n", valueIncrement);
    printf("\tInitial value: %d\n", initialValue);
    printf("To leave this menu, press [e]\n");
    printf("Would you like to change it? [Y/n] ");
    char answer, trash;
    scanf("%*c");
    int n, errorCode = 0;
    again:
    n = scanf("%c", &answer);
    if (n < 1 || answer == 'e')
        return 0;
    if (answer == 'Y' || answer == 'y') {
        printf("Input number of tests per iteration: ");
        errorCode = inputInt(&numberOfTestsPerIteration);
        if (errorCode || numberOfIterations < 1)
            return 1;

        printf("Input number of iterations: ");
        errorCode = inputInt(&numberOfIterations);
        if (errorCode || numberOfIterations < 1)
            return 1;

        printf("Input value increment: ");
        errorCode = inputInt(&valueIncrement);
        if (errorCode)
            return 1;

        printf("Input initial value: ");
        errorCode = inputInt(&initialValue);
        if (errorCode)
            return 1;
        return 0;
    }
    else if (answer != 'n') {
        printf(">>>Incorrect answer! Try again<<<\n");
        goto again;
    }

    errorCode = treeBenchmark(numberOfTestsPerIteration, numberOfIterations, valueIncrement, initialValue);
    
    return errorCode;
}
