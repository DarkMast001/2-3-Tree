#ifndef DIALOG_H
#define DIALOG_H

#include "Btree.h"

/*!
 * @brief Диалоговая функция добавления Node
 */
int dAdd(Tree *tree);

/*!
 * @brief Диалоговая функция удаления Node
 */
int dDelete(Tree *tree);

/*!
 * @brief Диалоговая функция обхода дерева
 */
int dTraversal(Tree *tree);

/*!
 * @brief Диалоговая функция поиска элемента
 */
int dFind(Tree *tree);

/*!
 * @brief Диалоговая функция специального поиска элемента
 */
int dSmartFind(Tree *tree);

/*!
 * @brief Диалоговая функция вывода дерева в стандартный поток вывода
 */
int dPrint(Tree *tree);

/*!
 * @brief Диалоговая функция cпециального вывода дерева в стандартный поток вывода
 */
int dSpecialPrint(Tree *tree);

/*!
 * @brief Диалоговая функция рисования дерва на картинке
 */
int dView(Tree *tree);

/*!
 * @brief Диалоговая функция начала бенчмарка
 */
int dBenchmark();

#endif
