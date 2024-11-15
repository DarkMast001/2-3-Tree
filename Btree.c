#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <readline/readline.h>
#include <time.h>

#include "usefulFuncs.h"
#include "Btree.h"

Tree* createTree(){
	Tree *tree = calloc(1, sizeof(Tree));
	tree->head = NULL;
	return tree;
}

int isLeaf(Node *node){
	return node->left == NULL && node->middle == NULL && node->right == NULL;
}

Node* createNode(){
	Node *node = calloc(1, sizeof(Node));
	if (node == NULL)
		return NULL;
	node->size = 0;
	node->left = NULL;
	node->middle = NULL;
	node->right = NULL;
	node->parent = NULL;
	return node;
}

Info *createInfo(){
	Info *info = calloc(1, sizeof(Info));
	if (info == NULL)
		return NULL;
	info->next = NULL;
	info->prev = NULL;
	info->str = NULL;
	return info;
}

void sortKeys(Node *node){
	if (node->size == 1)
		return;
	if (node->size == 2 && node->key[0] > node->key[1]){
		unsigned int tmpK = node->key[0];
		node->key[0] = node->key[1];
		node->key[1] = tmpK;

		Info *tmpI = node->info[0];
		node->info[0] = node->info[1];
		node->info[1] = tmpI;

		return;
	}
	if (node->size == 3){
		unsigned int tmpK;
		Info *tmpI;
		for(int i = 0; i < 3; i++){
			for(int j = 0; j < 2 - i; j++){
				if (node->key[j] > node->key[j + 1]){
					tmpK = node->key[j];
					node->key[j] = node->key[j + 1];
					node->key[j + 1] = tmpK;

					tmpI = node->info[j];
					node->info[j] = node->info[j + 1];
					node->info[j + 1] = tmpI;
				}
			}
		}
	}
}

int addNode(Tree *tree, unsigned int key, char *str){
	Node *currentNode = tree->head;

	while(1){

		// Проверяем дубликаты
		for(int i = 0; currentNode != NULL && i < currentNode->size; i++){
			if (currentNode->key[i] == key) {
				Info *info = createInfo();
				if (info == NULL)
					return 1;
				info->str = str;
				info->next = currentNode->info[i];
				currentNode->info[i]->prev = info;
				currentNode->info[i] = info;
				return 0;
			}
		}

		// Если дерево пустое
		if (tree->head == NULL) {
			tree->head = createNode();
			Info *info = createInfo();
			if (tree->head == NULL || info == NULL)
				return 1;
			tree->head->size = 1;
			tree->head->key[0] = key;
			info->str = str;
			tree->head->info[0] = info;
			break;
		}
		// Если текущий узел является листом
		else if (isLeaf(currentNode)) {
			// Ключей в массиве 1 или 2
			Info *info = createInfo();
			if (info == NULL)
				return 1;
			info->str = str;
			currentNode->size++;
			currentNode->key[currentNode->size - 1] = key;
			currentNode->info[currentNode->size - 1] = info;
			sortKeys(currentNode);

			// Если ключей в массиве 3, разделяем 
			if (currentNode->size == 3) {
				Node *leftNode = createNode();
				Node *rightNode = createNode();
				if (leftNode == NULL || rightNode == NULL)
					return 1;

				currentNode->size = 1;

				// Переносим всё в leftNode из currentNode : key[0] и info[0]
				leftNode->size = 1;
				leftNode->key[0] = currentNode->key[0];
				currentNode->key[0] = currentNode->key[1];
				leftNode->info[0] = currentNode->info[0];
				currentNode->info[0] = currentNode->info[1];
				leftNode->parent = currentNode;
				currentNode->left = leftNode;

				// Переносим всё в rightNode из currentNode : key[2] и info[2]
				rightNode->size = 1;
				rightNode->key[0] = currentNode->key[2];
				rightNode->info[0] = currentNode->info[2];
				rightNode->parent = currentNode;
				currentNode->right = rightNode;

				// Так как наш один узел, состоящий из 3-х ключей раздилися на 3 отдельных узла по 1 ключу,
				// то нам теперь надо ключ currentNode добавить к ключам в parent

				remerger: 
				if (currentNode->parent == NULL || currentNode->size == 2 && currentNode->parent->size == 1 || currentNode->size == 2 && currentNode->parent->size == 2)
					break;
				// Если в parent 1 ключ : 
				// 		возможно два случая, когда мы подступаемся к корневому узлу либо справа, либо слева
				if (currentNode->parent->size == 1) {
					currentNode->parent->size = 2;
					currentNode->parent->key[1] = currentNode->key[0];
					currentNode->parent->info[1] = currentNode->info[0];
					sortKeys(currentNode->parent);
					if (currentNode->left->key[0] > currentNode->parent->key[0]){
						currentNode->parent->middle = currentNode->left;
						currentNode->parent->right = currentNode->right;
						currentNode->right->parent = currentNode->parent;
						currentNode->left->parent = currentNode->parent;
					}
					else {
						currentNode->parent->middle = currentNode->right;
						currentNode->parent->left = currentNode->left;
						currentNode->right->parent = currentNode->parent;
						currentNode->left->parent = currentNode->parent;
					}
					Node *victim = currentNode;
					currentNode = currentNode->parent;
					free(victim);
				}
				// Если в parent 2 ключа :
				//		возможно три случая, когда мы подступаемся к корневому узлу слева/по центру/справа
				else if (currentNode->parent->size == 2) {
					// Если подходим справа
					if (currentNode->left->key[0] > currentNode->parent->key[1]) {
						Node *leftNewNode = createNode();
						leftNewNode->size = 1;
						leftNewNode->key[0] = currentNode->parent->key[0];
						leftNewNode->info[0] = currentNode->parent->info[0];
						leftNewNode->left = currentNode->parent->left;
						leftNewNode->left->parent = leftNewNode;
						leftNewNode->right = currentNode->parent->middle;
						leftNewNode->right->parent = leftNewNode;
						leftNewNode->parent = currentNode->parent;
						currentNode->parent->middle = NULL;
						currentNode->parent->left = leftNewNode;

						currentNode->parent->key[0] = currentNode->parent->key[1];
						currentNode->parent->info[0] = currentNode->parent->info[1];
						currentNode->parent->size = 1;
						currentNode = currentNode->parent;
					}
					// Если подходим слева
					else if (currentNode->right->key[0] < currentNode->parent->key[0]) {
						Node *rightNewNode = createNode();
						rightNewNode->size = 1;
						rightNewNode->key[0] = currentNode->parent->key[1];
						rightNewNode->info[0] = currentNode->parent->info[1];
						rightNewNode->right = currentNode->parent->right;
						rightNewNode->right->parent = rightNewNode;
						rightNewNode->left = currentNode->parent->middle;
						rightNewNode->left->parent = rightNewNode;
						rightNewNode->parent = currentNode->parent;
						currentNode->parent->middle = NULL;
						currentNode->parent->right = rightNewNode;

						currentNode->parent->size = 1;
						currentNode = currentNode->parent;
					}
					// Если подходим из центра
					else {
						currentNode->parent->size = 3;
						currentNode->parent->key[2] = currentNode->key[0];
						currentNode->parent->info[2] = currentNode->info[0];

						Node *leftNewNode = createNode();
						Node *rightNewNode = createNode();
						leftNewNode->size = 1;
						rightNewNode->size = 1;

						leftNewNode->key[0] = currentNode->parent->key[0];
						leftNewNode->info[0] = currentNode->parent->info[0];
						leftNewNode->left = currentNode->parent->left;
						leftNewNode->left->parent = leftNewNode;
						leftNewNode->right = currentNode->left;
						leftNewNode->right->parent = leftNewNode;
						leftNewNode->parent = currentNode->parent;
						currentNode->parent->left = leftNewNode;

						rightNewNode->key[0] = currentNode->parent->key[1];
						rightNewNode->info[0] = currentNode->parent->info[1];
						rightNewNode->right = currentNode->parent->right;
						rightNewNode->right->parent = rightNewNode;
						rightNewNode->left = currentNode->right;
						rightNewNode->left->parent = rightNewNode;
						rightNewNode->parent = currentNode->parent;
						currentNode->parent->right = rightNewNode;

						currentNode->parent->key[0] = currentNode->parent->key[2];
						currentNode->parent->info[0] = currentNode->parent->info[2];
						currentNode->parent->size = 1;

						currentNode->parent->middle = NULL;
						Node *victim = currentNode;
						currentNode = currentNode->parent;
						free(victim);
					}
				}
				// currentNode = currentNode->parent;
				if (currentNode->parent != NULL){
					goto remerger;
				}
			}

			break;
		}
		else if (key < currentNode->key[0]) 
			currentNode = currentNode->left;
		else if ((currentNode->size == 1) || (currentNode->size == 2 && key > currentNode->key[1]))
			currentNode = currentNode->right;
		else
			currentNode = currentNode->middle;
	}

	return 0;
}

void deleteTreeRec(Node *node){
	if (node != NULL){
		deleteTreeRec(node->left);
		deleteTreeRec(node->middle);
		deleteTreeRec(node->right);
		for(int i = 0; i < node->size; i++){
			if (node->info[i]->next != NULL){
				Info *ptr = node->info[i]->next, *ptrPrev;

				while(ptr != NULL){
					ptrPrev = ptr;
					free(ptr->str);
					ptr = ptr->next;
					free(ptrPrev);
				}
			}
			free(node->info[i]->str);
			free(node->info[i]);
		}
		free(node);
	}
}

void deleteTree(Tree *tree){
	deleteTreeRec(tree->head);
	free(tree);
}

void printTreeRec(Node *node, int n){
	if (node->right)
		printTreeRec(node->right, n + 1);

	for(int i = 0; i < n; i++){
		if (i == n - 1)
			printf("    |");
		else
			printf("     ");
	}
	printf("-[");
	for(int i = 0; i < node->size; i++){
		if (i == 0)
			printf("%d", node->key[i]);
		else
			printf(", %d", node->key[i]);
	}
	printf("]\n");

	if (node->middle)
		printTreeRec(node->middle, n + 1);
	if (node->left)
		printTreeRec(node->left, n + 1);
}

void printTreeUsualRec(Node *node){
	if (node != NULL) {
		for(int i = 0; i < node->size; i++){
			if (node->parent != NULL)
				if (node->parent->size == 2)
					printf("Key[%d]: %u - {%u, %u}\n", i + 1, node->key[i], node->parent->key[0], node->parent->key[1]);
				else
					printf("Key[%d]: %u - {%u}\n", i + 1, node->key[i], node->parent->key[0]);
			else
				printf("Key[%d]: %u\n", i + 1, node->key[i]);
			if (node->info[i]->next != NULL){
				Info *ptr = node->info[i];
				int j = 0;
				while(ptr != NULL){
					printf("\tInfo[%d]: %s\n", j + 1, ptr->str);
					ptr = ptr->next;
					j++;
				}
			}
			else
				printf("\tInfo: %s\n", node->info[i]->str);
		}
		printf("--------\n");
		printTreeUsualRec(node->left);
		printTreeUsualRec(node->middle);
		printTreeUsualRec(node->right);
	}
}

int printTree(Tree *tree){
	if (tree->head == NULL)
		return 1;

	printTreeRec(tree->head, 0);
	printf("---------------\n");
	printTreeUsualRec(tree->head);

	return 0;
}

void printReverseKeyOrderRecursion(Node *node){
	if (node != NULL){
		printReverseKeyOrderRecursion(node->left);
		if (node->size == 2){
			printf("Node: %u - %s\n", node->key[0], node->info[0]->str);
			printReverseKeyOrderRecursion(node->middle);
			printf("Node: %u - %s\n", node->key[1], node->info[1]->str);
		}
		else{
			printf("Node: %u - %s\n", node->key[0], node->info[0]->str);
		}
		printReverseKeyOrderRecursion(node->right);
	}
}

int printReverseKeyOrder(Tree *tree){
	if (tree->head == NULL)
		return 1;

	printf("---------------\n");
	printReverseKeyOrderRecursion(tree->head);

	return 0;
}

int importData(Tree **tree){
	if ((*tree)->head != NULL)
		deleteTree(*tree);
	else
		free(*tree);

	FILE *file;
	char str[50];
	char *estr;
	int i = 0, errorCode = 0;;

	file = fopen("input.txt", "r");
	if (!file){
		printf("Can't open file!\n");
		return 1;
	}

	unsigned int key;
	char *info;
	(*tree) = createTree();

	while(!feof(file)){
		fscanf(file, "%d", &key);
		estr = fgets(str, sizeof(str), file);
		if (estr == NULL)
			break;
		info = strdup(str);
		info[strlen(info) - 1] = '\0';
		if (i++ % 2 != 0){
			errorCode = addNode(*tree, key, info);
			if (errorCode)
				return 1;
		}
		else
			free(info);
	}
	free(estr);
	fclose(file);

	return 0;
}

void findInfoByKeyRec(Node *node, unsigned int key, Info **info){
	if (node != NULL) {
		for(int i = 0; i < node->size; i++){
			if (node->key[i] == key){
				(*info) = node->info[i];
			}
		}
		if (key < node->key[0]) 
			findInfoByKeyRec(node->left, key, info);
		else if ((node->size == 1) || (node->size == 2 && key > node->key[1]))
			findInfoByKeyRec(node->right, key, info);
		else
			findInfoByKeyRec(node->middle, key, info);
	}
}

Info* findInfoByKey(Tree *tree, unsigned int key){

	Info *info = NULL;
	findInfoByKeyRec(tree->head, key, &info);

	return info;
}

void smartFindNodeByKeyRec(Node *node, int keyLowerLimit, Info **info, unsigned int *min){
	if (node != NULL) {
		for(int i = 0; i < node->size; i++){
			if (node->key[i] <= *min && node->key[i] > keyLowerLimit){
				(*min) = node->key[i];
				node->info[i]->key = node->key[i];
				(*info) = node->info[i];
			}
		}
		//if (keyLowerLimit < node->key[0]) 
			smartFindNodeByKeyRec(node->left, keyLowerLimit, info, min);
		//else if ((node->size == 1) || (node->size == 2 && keyLowerLimit > node->key[1]))
			smartFindNodeByKeyRec(node->right, keyLowerLimit, info, min);
		//else
			smartFindNodeByKeyRec(node->middle, keyLowerLimit, info, min);
	}
}

Info* smartFindNodeByKey(Tree *tree, int keyLowerLimit){

	Info *info = NULL;
	unsigned int min = UINT_MAX;
	smartFindNodeByKeyRec(tree->head, keyLowerLimit, &info, &min);

	return info;
}

Node* findNodeByKey(Node *node, unsigned int key){
	if (!node)
		return NULL;

	for(int i = 0; i < node->size; i++){
		if (node->key[i] == key){
			return node;
		}
	}
	if (key < node->key[0]) 
		findNodeByKey(node->left, key);
	else if ((node->size == 1) || (node->size == 2 && key > node->key[1]))
		findNodeByKey(node->right, key);
	else
		findNodeByKey(node->middle, key);
}

Node* theMostLeftistOfRight(Node *node){
	if (!node)
		return node;
	if (!(node->left))
		return node;
	else return theMostLeftistOfRight(node->left);
}

void swapValue(Node *n1, Node *n2, int index){
	int tmp = n1->key[index];
	n1->key[index] = n2->key[0];
	n2->key[0] = tmp;

	Info *tmpI = n1->info[index];
	n1->info[index] = n2->info[0];
	n2->info[0] = tmpI;
}

void removeFromNode(Node *node, unsigned int key){
	if (node->size >= 1 && node->key[0] == key){
		node->key[0] = node->key[1];
		free(node->info[0]->str);
		free(node->info[0]);
		node->info[0] = node->info[1];
	}
	else if (node->size == 2 && node->key[1] == key) {
		free(node->info[1]->str);
		free(node->info[1]);
	}
	node->size--;
}

Info* makeCopy(Info *info){
	Info *infoNew = calloc(1, sizeof(Info));
	infoNew->str = strdup(info->str);
	infoNew->key = info->key;
	infoNew->next = info->next;
	infoNew->prev = info->prev;

	return infoNew;
}

Node* redistribute(Node *node){
	Node *parent = node->parent;
	Node *left = parent->left;
	Node *middle = parent->middle;
	Node *right = parent->right;

	if (parent->size == 1) {
		node->key[0] = parent->key[0];
		node->info[0] = parent->info[0];
		node->size++;

		if (left == node && right->size == 2) /*Проверил*/ {
			/**				 __						 __
			 * 				|20|					|30|
			 * 				/  \			->		/  \
			 * 			 |10|  |30|40|			 |20|  |40|
			 * 			  ^						
			 * 			  |						
			 */
			parent->key[0] = right->key[0];
			Info *infoCopy = makeCopy(right->info[0]);
			parent->info[0] = infoCopy;
			removeFromNode(right, right->key[0]);

			if (node->left == NULL){
				node->left = node->right;
			}

			node->right = right->left;
			right->left = right->middle;
			right->middle = NULL;
			if (node->right != NULL)
				node->right->parent = node;
		}
		else if (right == node && left->size == 2) /*Проверил*/ {
			/**				 __						 __
			 * 				|30|					|20|
			 * 				/  \			->		/  \
			 * 		  |10|20|  |40|		 		 |10|  |30|
			 * 			  		^						
			 * 			  		|						
			 */
			parent->key[0] = left->key[1];
			Info *infoCopy = makeCopy(left->info[1]);
			parent->info[0] = infoCopy;
			removeFromNode(left, left->key[1]);

			if (node->right == NULL){
				node->right = node->left;
			}

			node->left = left->right;
			left->right = left->middle;
			left->middle = NULL;
			if (node->left != NULL)
				node->left->parent = node;
		}
	}
	else if (parent->size == 2 && left->size < 2 && middle->size < 2 && right->size < 2) {
		if (node == left) /*Проверил*/ {
			/**				 __ __					 	 __
			 * 				|30|40|						|40|
			 * 				/  |  \			->			/  \
			 * 		  	 |20| |35||50|		 	  |30|35|  |50|
			 * 			  ^
			 * 			  |
			 */
			parent->left = parent->middle;
			parent->middle = NULL;
			parent->left->key[1] = parent->left->key[0];
			parent->left->info[1] = parent->left->info[0];
			parent->left->size++;
			parent->left->key[0] = parent->key[0];
			Info *infoCopy = makeCopy(parent->info[0]);
			parent->left->info[0] = infoCopy;
			parent->left->middle = parent->left->left;

			if (node->left != NULL)
				parent->left->left = node->left;
			else if (node->right != NULL)
				parent->left->left = node->right;

			if (parent->left->left != NULL)
				parent->left->left->parent = parent->left;

			removeFromNode(parent, parent->key[0]);
			free(node);
		}
		else if (node == middle) /*Проверил*/ {
			/**				 __ __					 	 __
			 * 				|30|40|						|40|
			 * 				/  |  \			->			/  \
			 * 		  	 |20| |35||50|		 	  |20|30|  |50|
			 * 			  	   ^
			 * 			       |
			 */
			parent->middle = NULL;
			parent->left->key[1] = parent->key[0];
			Info *infoCopy = makeCopy(parent->info[0]);
			parent->left->info[1] = infoCopy;
			parent->left->size++;
			parent->left->middle = parent->left->right;

			if (node->right != NULL)
				left->right = node->right;
			else if (node->left != NULL)
				left->right = node->left;

			if (left->right != NULL)
				left->right->parent = left;

			removeFromNode(parent, parent->key[0]);
			free(node);
		}
		else if (node == right) /*Проверил*/ {
			/**				 __ __					 	 __
			 * 				|30|40|						|30|
			 * 				/  |  \			->			/  \
			 * 		  	 |20| |35||50|		 	  	 |20|  |35|40|
			 * 			  	   	   ^
			 * 			       	   |
			 */
			parent->right = parent->middle;
			parent->middle = NULL;
			parent->right->key[1] = parent->key[1];
			Info *infoCopy = makeCopy(parent->info[1]);
			parent->right->info[1] = infoCopy;
			parent->right->size++;
			parent->right->middle = parent->right->right;

			if (node->left != NULL)
				parent->right->right = node->left;
			else if (node->right != NULL)
				parent->right->right = node->right;

			if (parent->right->right != NULL)
				parent->right->right->parent = parent->right;	//// CHECK

			removeFromNode(parent, parent->key[1]);
			free(node);
		}
	}
	else if (parent->size == 2 && (left->size == 2 || middle->size == 2 || right->size == 2)) {
		// Это условие разделяется ещё на 3 случая, 2 из которых можно объеденить
		if (node == left) {
			if (node->left == NULL){
				node->left = node->right;
				node->right = NULL;
			}
			node->key[0] = parent->key[0];
			node->info[0] = parent->info[0];
			node->size++;
			if ((middle->size == 2 && right->size == 2) || (middle->size == 2 && right->size == 1)) /*Проверил*/ {
				/**				   __     __					 	   __     __
				 * 				|  20  |  40  |						|  25  |  40  |
				 * 				/      |      \			->			/  	   |	  \
				 * 		  	 |10|   |25|35|   |45|55|		 	 |20|  	  |35|	  |45|55|
				 * 			  ^
				 * 			  |
				 */
				/**				   __     __					 	   __     __
				 * 				|  20  |  40  |						|  25  |  40  |
				 * 				/      |      \			->			/  	   |	  \
				 * 		  	 |10|   |25|35|   |55|		 	 	 |20|  	  |35|	  |55|
				 * 			  ^
				 * 			  |
				 */
				parent->key[0] = parent->middle->key[0];
				Info *infoCopy = makeCopy(parent->middle->info[0]);
				parent->info[0] = infoCopy;
				removeFromNode(parent->middle, parent->middle->key[0]);

				left->right = middle->left;
				if (left->right != NULL)
					left->right->parent = left;
				middle->left = middle->middle;
				middle->middle = NULL;
			}
			else if (middle->size == 1 && right->size == 2) /*Проверил*/ {
				/**				   __     __					 	   __     __	
				 * 				|  20  |  40  |						|  30  |  45  |
				 * 				/      |      \			->			/      |      \
				 * 		  	 |10|     |30|    |45|55|		  	 |20|     |40|    |55|
				 * 			  ^
				 * 			  |
				 */
				parent->key[0] = middle->key[0];
				parent->info[0] = middle->info[0];
				middle->key[0] = parent->key[1];
				middle->info[0] = parent->info[1];
				parent->key[1] = right->key[0];
				Info *infoCopy = makeCopy(right->info[0]);
				parent->info[1] = infoCopy;

				left->right = middle->left;
				if (left->right != NULL)
					left->right->parent = left;
				middle->left = middle->right;
				middle->right = right->left;
				if (middle->right != NULL)
					middle->right->parent = middle;
				right->left = right->middle;
				right->middle = NULL;

				removeFromNode(right, right->key[0]);
			}
		}
		// Это условие разделяется ещё на 3 случая, 2 из которых можно объеденить
		else if (node == middle) {
			if ((left->size == 2 && right->size == 2) || (left->size == 2 && right->size == 1)) /*Проверил*/ {
				/**				   __     __					 	   __     __
				 * 				|  20  |  40  |						|  10  |  40  |
				 * 				/      |      \			->			/  	   |	  \
				 * 		   |5|10|	  |30|    |50|60|		 	  |5|  	  |20|	  |50|60|
				 * 					   ^
				 * 					   |
				 */
				/**				   __     __					 	   __     __
				 * 				|  20  |  40  |						|  10  |  40  |
				 * 				/      |      \			->			/  	   |	  \
				 * 		   |5|10|	  |30|    |50|		 		  |5|	  |20|	  |50|
				 * 					   ^
				 * 					   |
				 */
				if (node->right == NULL){
					node->right = node->left;
					node->left = NULL;
				}
				node->key[0] = parent->key[0];
				node->info[0] = parent->info[0];
				node->size++;
				parent->key[0] = parent->left->key[1];
				Info *infoCopy = makeCopy(parent->left->info[1]);
				parent->info[0] = infoCopy;
				removeFromNode(parent->left, parent->left->key[1]);

				middle->left = left->right;
				if (middle->left != NULL)
					middle->left->parent = middle;
				left->right = left->middle;
				left->middle = NULL;
			}
			else if (left->size == 1 && right->size == 2) /*Проверил*/ {
				/**				   __     __					 	   __     __
				 * 				|  20  |  40  |						|  20  |  50  |
				 * 				/      |      \			->			/  	   |	  \
				 * 		   	 |10|	  |30|    |50|60|		 	 |10|  	  |40|	  |60|
				 * 					   ^
				 * 					   |
				 */
				if (node->left == NULL){
					node->left = node->right;
					node->right = NULL;
				}
				middle->key[0] = parent->key[1];
				middle->info[0] = parent->info[1];
				middle->size++;
				parent->key[1] = right->key[0];
				Info *infoCopy = makeCopy(right->info[0]);
				parent->info[1] = infoCopy;
				removeFromNode(right, right->key[0]);

				middle->right = right->left;
				if (middle->right != NULL)
					middle->right->parent = middle;
				right->left = right->middle;
				right->middle = NULL;
			}
		}
		// Это условие разделяется ещё на 3 случая, 2 из которых можно объеденить
		else if (node == right) {
			if (node->left != NULL){
				node->right = node->left;
				node->left = NULL;
			}

			node->key[0] = parent->key[1];
			node->info[0] = parent->info[1];
			node->size++;
			if ((left->size == 2 && middle->size == 2) || (left->size == 1 && middle->size == 2)) /*Проверил*/ {
				/**				   __     __					 	   __     __	
				 * 				|  20  |  40  |						|  20  |  35  |
				 * 				/      |      \			->			/      |      \
				 * 		   |5|10|	|25|35|   |50|		 	   |5|10|	  |25|    |40|
				 * 					   		   ^
				 * 					   		   |
				 */
				/**				   __     __					 	   __     __	
				 * 				|  20  |  40  |						|  20  |  35  |
				 * 				/      |      \			->			/      |      \
				 * 		   	 |10|	|25|35|   |50|		 	  	 |10|	  |25|    |40|
				 * 					   		   ^
				 * 					   		   |
				 */
				parent->key[1] = parent->middle->key[1];
				Info *infoCopy = makeCopy(parent->middle->info[1]);
				parent->info[1] = infoCopy;
				removeFromNode(parent->middle, parent->middle->key[1]);

				node->left = middle->right;
				middle->right = middle->middle;
				if (node->left != NULL)
					node->left->parent = node;
				middle->middle = NULL;
			}
			else if (left->size == 2 && middle->size == 1) /*Проверил*/ {
				/**				   __     __					 	   __     __	
				 * 				|  20  |  40  |						|  10  |  35  |
				 * 				/      |      \			->			/      |      \
				 * 		   |5|10|	  |35|    |50|		 	   	  |5|	  |20|    |40|
				 * 					   		   ^
				 * 					   		   |
				 */
				parent->key[1] = middle->key[0];
				parent->info[1] = middle->info[0];

				node->left = middle->right;
				middle->right = middle->left;
				if (node->left != NULL)
					node->left->parent = node;

				middle->key[0] = parent->key[0];
				middle->info[0] = parent->info[0];
				parent->key[0] = left->key[1];
				Info *infoCopy = makeCopy(left->info[1]);
				parent->info[0] = infoCopy;
				removeFromNode(left, left->key[1]);

				middle->left = left->right;
				if (middle->left != NULL)
					middle->left->parent = middle;
				left->right = left->middle;
				left->middle = NULL;
			}
		}
	}

	return parent;
}

Node *merge(Tree *tree, Node *node){
	Node *parent = node->parent;

	if (parent->left == node) {
		parent->right->key[1] = parent->right->key[0];
		parent->right->info[1] = parent->right->info[0];
		parent->right->size++;
		parent->right->key[0] = parent->key[0];
		Info *infoCopy = makeCopy(parent->info[0]);
		parent->right->info[0] = infoCopy;

		parent->right->middle = parent->right->left;

		if (node->right != NULL)
			parent->right->left = node->right;
		else if (node->left != NULL)
			parent->right->left = node->left;

		if (parent->right->left != NULL)
			parent->right->left->parent = parent->right;
		else
			parent->right->middle = NULL;

		removeFromNode(parent, parent->key[0]);
		parent->left = NULL;
		free(node);
	}
	else if (parent->right == node) {
		parent->left->key[1] = parent->key[0];
		Info *infoCopy = makeCopy(parent->info[0]);
		parent->left->info[1] = infoCopy;
		parent->left->size++;

		parent->left->middle = parent->left->right;

		if (node->right != NULL)
			parent->left->right = node->right;
		else if (node->left != NULL)
			parent->left->right = node->left;

		if (parent->left->right != NULL)
			parent->left->right->parent = parent->left;
		else
			parent->left->middle = NULL;

		removeFromNode(parent, parent->key[0]);
		parent->right = NULL;
		free(node);
	}

	if (parent->parent == NULL) {
		Node *tmp = NULL;
		if (parent->right != NULL)
			tmp = parent->right;
		else
			tmp = parent->left;
		tmp->parent = NULL;
		free(parent);
		tree->head = tmp;
		return tmp;
	}

	return parent;
}

void fix(Tree *tree, Node *node){
	// 1 случай : удяляем единственный ключ в дереве
	if (node->size == 0 && node->parent == NULL){
		free(node);
		tree->head = NULL;
		return;
	}

	// 2 случай : вершина, в которой удалили ключ, имела два ключа
	if (node->size != 0) {
		if (node->parent)
			return fix(tree, node->parent);
		else
			return;
	}

	// Оставшиеся случаи для node->size == 0
	// 3 случай : когда хотя бы у одного из братьев есть 2 ключа, то нужно распределить ключи
	Node *parent = node->parent;
	Node *first = parent->left;
	Node *second = (parent->middle == NULL) ? parent->right : parent->middle;
	Node *third = (second == parent->right) ? NULL : parent->right;
	if (first->size == 2 || second->size == 2 || parent->size == 2)
		node = redistribute(node);
	else if (parent->size == 2 && third->size == 2)
		node = redistribute(node);
	// 4 случай : нужно произвести склеивание и пройти по дереву на ещё одну ветвь вверх как минимум
	else
		node = merge(tree, node);

	return fix(tree, node);
}

int deleteNode(Tree *tree, unsigned int key, int benchmark){
	Node *node = findNodeByKey(tree->head, key);
	if (!node)
		return 1;

	// Если лежит несколько элементов по этому ключу
	for(int i = 0; i < node->size; i++){
		if (node->key[i] == key && node->info[i]->next != NULL) {
			Info *infoElement = node->info[i];
			int serial, errorCode;
			if (benchmark == 1){
				serial = 1;
				goto label;
			}
			printf("There are several values under this key, enter the serial number of the information: ");
			errorCode = inputInt(&serial);
			if (errorCode)
        		return 1;
        	label:
        	for(int j = 1; infoElement != NULL; j++){
		        if (j == serial) {
		        	Info *victim = infoElement;
		        	if (infoElement->prev == NULL) {
		        		infoElement->next->prev = NULL;
		        		node->info[i] = infoElement->next;
		        	}
		        	else if (infoElement->next == NULL)
		        		infoElement->prev->next = NULL;
		        	else {
		        		infoElement->prev->next = infoElement->next;
		        		infoElement->next->prev = infoElement->prev;
		        	}
		        	free(victim->str);
	        		free(victim);
		            return 0;
		        }
		        infoElement = infoElement->next;
		    }
		}
	}

	// Если по этому ключу 1 элемент
	Node *minNode = NULL;
	Node *first = node->left;
	Node *second = (node->middle == NULL) ? node->right : node->middle;
	Node *third = (second == node->right) ? NULL : node->right;
	if (node->key[0] == key)
		minNode = theMostLeftistOfRight(second);
	else
		minNode = theMostLeftistOfRight(third);

	if (minNode) {
		int index = (key == node->key[0] ? 0 : 1);
		swapValue(node, minNode, index);
		node = minNode;
	}

	removeFromNode(node, key);
	fix(tree, node);
	// for(int i = 0; i < node->size; i++){
	// 	printf("%u\n", node->key[i]);
	// }
	// printf("---------\n");
	// for(int i = 0; i < minNode->size; i++){
	// 	printf("%u\n", minNode->key[i]);
	// }

	return 0;
}

void freeNode(Node *node){
	for(int i = 0; i < node->size; i++){
		if (node->info[i]->next != NULL){
			Info *ptr = node->info[i]->next, *ptrPrev;

			while(ptr != NULL){
				ptrPrev = ptr;
				free(ptr->str);
				ptr = ptr->next;
				free(ptrPrev);
			}
		}
		free(node->info[i]->str);
		free(node->info[i]);
	}
}

int treeBenchmark(int numberOfTestsPerIteration, int numberOfIterations, int valueIncrement, int initialValue){

	clock_t startAdd = 0, endAdd = 0, startFind = 0, endFind = 0, startDelete = 0, endDelete = 0;
	clock_t averageAdd = 0, averageFind = 0, averageDelete = 0;

	int errorCode = 0;
	for(int i = 0; i < numberOfIterations; i++){
		srand(time(NULL));
		for(int j = 0; j < numberOfTestsPerIteration; j++){
			Tree *tree = createTree();

			startAdd = clock();
			for(int k = 0; k < initialValue; k++){
				unsigned int key = (unsigned int)(rand() % initialValue);
				errorCode = addNode(tree, key, strdup("lol"));
				if (errorCode)
					return 1;
			}
			endAdd = clock();
			averageAdd += (endAdd - startAdd);

			startFind = clock();
			for(int k = 0; k < initialValue; k++){
				Node *a = findNodeByKey(tree->head, rand() % initialValue);
			}
			endFind = clock();
			averageFind += (endFind - startFind);

			startDelete = clock();
			for(int k = 0; k < initialValue; k++){
				unsigned int key = (unsigned int)(rand() % initialValue);
				deleteNode(tree, key, 1);
				// if (errorCode)
				// 	printf("ERROR!\n");
			}
			endDelete = clock();
			averageDelete += (endDelete - startDelete);

			deleteTree(tree);
		}
		printf("-----------------------------------------\n");
		printf("Average time adding (%d elements) - %lf\n", initialValue, (double)(averageAdd / numberOfIterations) / CLOCKS_PER_SEC);
		printf("Average time searching (%d elements) - %lf\n", initialValue, (double)(averageFind / numberOfIterations) / CLOCKS_PER_SEC);
		printf("Average time deleting (%d elements) - %lf\n", initialValue, (double)(averageDelete / numberOfIterations) / CLOCKS_PER_SEC);

		initialValue += valueIncrement;
	}

	return 0;
}