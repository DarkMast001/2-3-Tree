#ifndef USEFULFUNCS_H
#define USEFULFUNCS_H

/*!
 * @brief Функция записывает в n число типа int
 * 
 * @param *n Указатель на переменную, в которую будет записано int число
 * 
 * @return 0, если успешно удалось записать в n число, иначе 1
 */
int inputInt(int *n);

/*!
 * @brief Функция записывает в n число типа unsigned int
 * 
 * @param *n Указатель на переменную, в которую будет записано unsigned int число
 * 
 * @return 0, если успешно удалось записать в n число, иначе 1
 */
int inputUnsigned(unsigned int *n);

/*!
 * @brief Функция выбора диалоговой функции
 * 
 * @param *msfs[] Массив указателей на диалоговые функции
 * @param nMsgs Количесвто диалоговых функций
 * 
 * @return Индекс указателя на диалоговую функцию из массива msfs. Если таковой не найдено, то возвращается -1, если обнаружен EOF, то вернёт 0
 */
int dialog(const char *msgs[], const int nMsgs);

#endif