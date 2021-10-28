#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define RAZR_LEN_OF_UINT1024 35
#define BASE 1000000000
#define DIGITS 9

typedef struct { /*1120 бит - 140 байт*/
    unsigned int arr[RAZR_LEN_OF_UINT1024];
} uint1024_t;

void shift_left(uint1024_t *unit, unsigned int default_value);
uint1024_t multiply_by_uint(uint1024_t *multip, unsigned int uint);


void printf_value(uint1024_t *x) {
    char i;
    bool leading_zeroes = true;
    for (i = RAZR_LEN_OF_UINT1024 - 1; i >= 0; i--) {
        if ((leading_zeroes) && (x->arr[i] != 0)) {
            leading_zeroes = false;
            printf("%u", x->arr[i]);
            continue;
        }
        if (!leading_zeroes) {
            printf("%09u", x->arr[i]);
        }
    }
    if (leading_zeroes) {
        printf("0");
    }
    putchar('\n'); //Переход на новую строку!!
}

void scanf_value(uint1024_t *x) {
    bool leading_zeroes = true;
    char i;
    for (i = 0; i < RAZR_LEN_OF_UINT1024; i++) {
        x->arr[i] = 0;
    }
    char symbol;
    char bufer[DIGITS + 1];
    bufer[DIGITS] = '\0';
    char count = 0;
    symbol = (char) getchar();
    while (symbol != '\n') {
        if ((leading_zeroes)) {
            if (symbol == '0') {
                symbol = (char) getchar();
                continue;
            } else {
                leading_zeroes = false;
            }
        }
        bufer[count] = symbol;
        count++;
        if (count == DIGITS) {
            shift_left(x, atoi(bufer));
            count = 0;
        }
        symbol = (char) getchar();
    }
    if (count > 0) {
        char add_bufer[DIGITS];

        for (i = 0; i < count; i++) {
            add_bufer[i] = bufer[i];
        }
        add_bufer[count] = '\0';
        shift_left(x, atoi(add_bufer));
    }
    if ((x->arr[0] / (BASE / 10) == 0) && (count > 0) && (!leading_zeroes)) {
        unsigned int base_diff = BASE, transfer_prev = 0, transfer;
        for (i = DIGITS; i > count; i--) {
            base_diff = base_diff / 10;
        }
        for (i = RAZR_LEN_OF_UINT1024 - 1; i > 0; i--) {
            transfer = x->arr[i] % (BASE / base_diff);
            x->arr[i] = x->arr[i] / (BASE / base_diff);
            x->arr[i] = x->arr[i] + transfer_prev * (base_diff);
            transfer_prev = transfer;
        }
        x->arr[0] = x->arr[0] + transfer_prev * (base_diff);
    }
}


uint1024_t from_uint(unsigned int x) {
    uint1024_t answer;
    for (char i = 0; i < RAZR_LEN_OF_UINT1024; i++) {
        answer.arr[i] = x % BASE;
        x = x / BASE;
    }
    return answer;
}

uint1024_t add_op(uint1024_t x, uint1024_t y) {
    uint1024_t answer;
    unsigned int temp;
    unsigned char transfer = 0;
    for (char i = 0; i < RAZR_LEN_OF_UINT1024; i++) {
        temp = x.arr[i] + y.arr[i] + transfer;
        answer.arr[i] = temp % BASE;
        transfer = temp / BASE;
    }
    return answer;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y) {
    uint1024_t answer;
    for (char i = 0; i < RAZR_LEN_OF_UINT1024; i++) {
        if (x.arr[i] >= y.arr[i]) {
            answer.arr[i] = x.arr[i] - y.arr[i];
        }
        else {
            answer.arr[i] = BASE - (y.arr[i] - x.arr[i]);
            for (unsigned char j = i + 1; j < RAZR_LEN_OF_UINT1024; j++) {
                if (x.arr[j] == 0) {
                    x.arr[j] = BASE - 1;
                    continue;
                } else {
                    x.arr[j]--;
                    break;
                }
            }
        }
    }
    return answer;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y) {
    uint1024_t answer = from_uint(0);
    bool leading_zeroes = true;
    for (char i = RAZR_LEN_OF_UINT1024 - 1; i >= 0; i--) {
        if ((leading_zeroes) && (y.arr[i] != 0)) {
            leading_zeroes = false;
        }
        if (!leading_zeroes) {
            shift_left(&answer, 0);
            answer = add_op(answer, multiply_by_uint(&x, y.arr[i]));
        }
    }
    return answer;
}

int main() {
    uint1024_t num1, num2, num_res;
    printf("Введите 1-е число: \n");
    scanf_value(&num1);
    printf("Введите 2-е число \n");
    scanf_value(&num2);
    printf("Вы ввели числа:\n");
    printf_value(&num1);
    printf("и\n");
    printf_value(&num2);
    printf("их сумма:\n");
    num_res = add_op(num1, num2);
    printf_value(&num_res);
    printf("их разность:\n");
    num_res = subtr_op(num1, num2);
    printf_value(&num_res);
    printf("их произведение:\n");
    num_res = mult_op(num1, num2);
    printf_value(&num_res);
    return 0;
}

void shift_left(uint1024_t *unit, unsigned int default_value) {
    for (char i = RAZR_LEN_OF_UINT1024 - 1; i > 0; i--) {
        unit->arr[i] = unit->arr[i-1];
    }
    unit->arr[0] = default_value;
}

uint1024_t multiply_by_uint(uint1024_t *multip, unsigned int uint) {
    uint1024_t result = from_uint(0);
    bool leading_zeroes = true;
    char i;
    for (i=RAZR_LEN_OF_UINT1024-1; i>=0; i--) {
        if (multip->arr[i] != 0) {
            leading_zeroes = false;
            break;
        }
    }
    if (leading_zeroes) {
        return result;
    }
    unsigned int transfer = 0;
    unsigned long long int temp;
    for (char j=0; j <= i+1; j++) {
        temp = (unsigned long long int)multip->arr[j] * uint + transfer;
        result.arr[j] = temp % BASE;
        transfer = temp/BASE;
    }
    return result;
}