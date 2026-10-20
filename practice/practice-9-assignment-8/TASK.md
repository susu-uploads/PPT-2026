---
ru: "Задание №8"
en: "Assignment 8"
code: "practice-9-assignment-8"
origin: "https://edu.susu.ru/mod/assign/view.php?id=8720542"
---

Изучите приведенную ниже простейшую программу для GPU.
Изучите спецификатор \_\_global\_\_ , и конструкцию запуска ядра \<\<\<...\>\>\> .
Не забудьте, что расширение файла должно быть .cu .

#include \<stdio.h\>

\_\_global\_\_ void HelloKernel()
{
   printf("Hello world!");
}

int main()
{
   HelloKernel\<\<\<1, 1\>\>\>();
   return 0;
}

Соберите и запустите программу:
$ nvcc main.cu -o main
$ ./main
