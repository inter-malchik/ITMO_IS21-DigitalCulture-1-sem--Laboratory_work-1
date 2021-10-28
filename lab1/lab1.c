#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void helping() {
    printf("Вы позвали на помощь!\n\n");
    printf("утилита WordCount\n\n");
    printf("утилита WordCount подсчитывает количество строк, слов и байт для указанного файла.\n");
    printf("Название файла и опции передаются через аргументы командной строки в следующем формате:\n\n");
    printf("WordCount.exe [OPTION] filename\n\n");
    printf("где\tfilename - название рассматриваемого файла (файл должен существовать)\nа\t[OPTION] - это опции\n\n");
    printf("поддерживаемые опции:\n");
    printf("-h, --help\tвызов помощи (только если это единственный аргумент)\n");
    printf("-l, --lines\tвывод количества строк в файле\n");
    printf("-c, --bytes\tвывод размера файла в байтах\n");
    printf("-w, --words\tвывод количества слов в файле");
    printf("\n\nпри выборе нескольких опций, результат выводится в порядке\nbytes -> lines -> words");
}

bool check_file_accuracy(char *name) { /* функция проверки назв. файла на корректность */
    if (strchr(name, 46) > 0) {
        return true;
    }
    return false;
}

bool check_argument(char *argument, char *option) { /* функция проверки соответствия аргумента командной строки */
    if (strcmp(argument, option) == 0) {
        return true;
    }
    return false;
}

int main(int argv, char **argc) {
    char *filename = argc[argv - 1];
    if (argv <= 1) {
        printf("\aвы не ввели аргументов");
        return 0;
    } else if (argv == 2) {
        if ((check_argument(argc[1], "--help")) || (check_argument(argc[1], "-h"))) {
            helping();
            return 0;
        } else if (check_file_accuracy(filename)) {
            printf("\aвы не ввели аргументов для файла %s", filename);
            return 0;
        } else {
            printf("\aневерный ввод");
            return 0;
        }
    } else {
        printf("файл %s\n", filename);
        if (check_file_accuracy(filename)) {
            bool byte = false;
            bool line = false;
            bool word = false;
            for (int i = 1; i < argv; i++) {
                if ((check_argument(argc[i], "--bytes")) || (check_argument(argc[i], "-c"))) { byte = true; }
                if ((check_argument(argc[i], "--lines")) || (check_argument(argc[i], "-l"))) { line = true; }
                if ((check_argument(argc[i], "--words")) || (check_argument(argc[i], "-w"))) { word = true; }
            }
            if ((byte) || (line) || (word)) {
                FILE *file;
                file = fopen(filename, "r");
                if (file == NULL) {
                    printf("\aфайла не существует!");
                    return 0;
                }
                if (byte) {
                    fseek(file, 0L, SEEK_END);
                    unsigned long int size_of_file = ftell(file);
                    printf("размер файла: %lu байт\n", size_of_file);
                    rewind(file);
                }
                if ((line) || (word)) {
                    int amount_of_lines = 1;
                    long int amount_of_words = 0;
                    int c;
                    int c_prev = 32;
                    c = fgetc(file);
                    while (c != EOF) {
                        if (c == 10) {
                            amount_of_lines = amount_of_lines + 1;
                        }
                        if (((c == 10) || (c == 32)) && ((c_prev != 10) && (c_prev != 32))) {
                            amount_of_words = amount_of_words + 1;
                        }
                        c_prev = c;
                        c = fgetc(file);
                    }
                    if ((c != 10) && (c != 32)) {
                        amount_of_words = amount_of_words + 1;
                    }
                    if (line) { printf("строк в файле: %d\n", amount_of_lines); }
                    if (word) { printf("слов в файле: %ld\n", amount_of_words); }

                }
                fclose(file);
            } else {
                printf("\aвы не ввели корректных аргументов для файла %s", filename);
                return 0;
            }


        } else {
            printf("\aформат введенного файла некорректен!");
            return 0;
        }
    }
    return 0;
}
