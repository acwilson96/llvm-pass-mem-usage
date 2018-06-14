#include <stdio.h>

char char8bits;
int  int32bits;

int myFunc() {
    int x = 5;
    int y = 10;
    return x + y;
}

int main() {
    char8bits = 'a';
    int32bits = 32;
    int local32bits = 5;
    int funcRet     = myFunc();
    printf("string length 16");
}