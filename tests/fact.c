#include <stdio.h>

int a = 1;
int bb = 2;
int ccc = 3;
int dddd = 4;

int fact(int num) {
    if (num == 0)
        return 0;
    else if (num == 1)
        return 1;
    else
        return num * fact(num - 1);
}

int main() {
    int val = fact(5);
    printf("%d", val);
}