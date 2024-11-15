#ifndef BTREE_H_
#define BTREE_H_

/**
 * @page Tree Функции прямого взаимодействия с бинарным деревом
 */

/**
 * @brief Структура, хранящая в себе информацию
 * @details В случае, когда ключи будут совпадать, то Info превращается в список информаций по конкретному ключу
 * 
 * @code
 * typedef struct Info {
   * char *str;
   * struct Info *next;
   * struct Info *prev;
   * unsigned int key;
 * } Info;
 * @endcode
 */
typedef struct Info {
   char *str;           ///< Информационное поле
   struct Info *next;   ///< Указатель на следующий элемент списка
   struct Info *prev;   ///< Указывает на предыдущий элемент списка
   unsigned int key;
} Info;

/**
 * @brief Структура, хранящая в себе указатель на правое, среднее и левое поддерево и массив указателей на Info
 * @code
 * typedef struct Node {
    * int size;
    * unsigned int key[3];
 	* struct Node *left;
 	* struct Node *midle;
 	* struct Node *right;
 	* struct Node *parent;
 	* Info *info[3];
 * } Node;
 * @endcode
 */
typedef struct Node {
	int size;                 ///< Количество занятых ключей
	unsigned int key[3];      ///< Поле для хранения ключей узла
	struct Node *left;        ///< Указатель на левое поддерево
	struct Node *middle;      ///< Указатель на среднее поддерево
	struct Node *right;       ///< Указатель на правое поддерево
	struct Node *parent;      ///< Указатель на родительский узел
	Info *info[3];            ///< Указатели на информацию
} Node;

/**
 * @brief Структура, хранящая в себе указатель на первый Node созданного дерева
 * @code
 * typedef struct Tree {
 	* Node *head;
 * } Tree;
 * @endcode
 */
typedef struct Tree {
	Node *head;               ///< Указатель на корень дерева
} Tree;

/**
 * @brief Функция cоздаёт дерево
 * 
 * @return Указатель на созданное дерево
 */
Tree *createTree();

/**
 * @brief Функция добавляет в дерево элемент
 * 
 * @param *tree Указатель на созданное дерево
 * @param key Ключ создаваемого элемента
 * @param str Информация, которая будет храниться по заданному key
 * 
 * @return 0, если успешно удалось добавить ветвь дерева, иначе 1
 */
int addNode(Tree *tree, unsigned int key, char *str);

/**
 * @brief Функция очищает созданное дерево
 * 
 * @param *tree Указатель на созданное дерево
 * 
 * @return 0, если успешно удалось очистить дерево, иначе 1
 */
void deleteTree(Tree *tree);

/**
 * @brief Функция печатает дерево
 * 
 * @param *tree Указатель на созданное дерево
 * 
 * @return 0, если успешно удалось напечатать дерево, 1 - если дерево пустое
 */
int printTree(Tree *tree);

/**
 * @brief Функция печатает дерево в обратном порядке следования ключей
 * 
 * @param *tree Указатель на созданное дерево
 * 
 * @return 0, если успешно удалось напечатать дерево, 1 - если дерево пустое
 */
int printReverseKeyOrder(Tree *tree);

/**
 * @brief Функция импортирует из файла дерево
 * 
 * @param **tree Указатель на дерево, которое будет создано
 * 
 * @return 0, если успешно удалось добавить ветвь дерева, иначе 1
 */
int importData(Tree **tree);

/**
 * @brief Функция ищет Node по заданному ключу
 * 
 * @param *tree Указатель на дерево, которое уже создано
 * @param key Ключ, по которому будет выполнятся поиск
 * 
 * @return Указатель на Info. Eсли ничего не найдено, то вернёт NULL
 */
Info* findInfoByKey(Tree *tree, unsigned int key);

/**
 * @brief Функция удаляет элемент под заданным ключом key
 * 
 * @param *tree Указатель на дерево, которое уже создано
 * @param key Ключ, по которому будет выполнятся удаление
 * @param benchmark Флаг, который определяет то, как будет запущена функция удаления элемента. 0 - обычный режим, 1 - в режиме теста
 * 
 * @return 0, если успешно удалось удалить элемент дерева, иначе 1
 */
int deleteNode(Tree *tree, unsigned int key, int benchmark);

/**
 * @brief Поиск Node с наименьшим значением ключа, не превышающим keyUpperLimit
 * 
 * @param *tree Указатель на дерево, которое уже создано
 * @param keyUpperLimit Ключ, до которого будет выполнен поиск. То есть все ключи, значения которых максимальны, но меньше, чем keyUpperLimit, будут возвращены
 * 
 * @return Указатель на Info. Eсли ничего не найдено, то вернёт NULL
 */
Info* smartFindNodeByKey(Tree *tree, int keyUpperLimit);

/**
 * @brief Стресс тест дерева
 * @details Эта функция создаёт дерево на больших данных и вычисляет время выполнения функции поиска и удаления элемета
 * 
 * @param *tree Указатель на дерево
 * @param numberOfTestsPerIteration Количество раз выполнения за итерацию функции поиска и удаления
 * @param numberOfIterations Количество итераций
 * @param valueIncrement Приращение значения. На каждой итерации, количество чисел, на которых это дерево будет тестироваться, будет увеличиваться на это значение
 * @param initialValue Начальное количество значений, на которых будет тестироваться дерево
 *  
 * @return 0, если тест пройден успешно, 1 - если возникли ошибки или переполнения
 */
int treeBenchmark(int numberOfTestsPerIteration, int numberOfIterations, int valueIncrement, int initialValue);

#endif /* BTREE_H_ */
