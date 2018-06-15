#include <stdio.h>

char char8bits;
int  int32bits;

int myFunc2() {
    int x = 5;
    int y = 10;
    int z = 15;
    return x + y + z;
}

int myFunc() {
    int x = 5;
    int y = 10;
    return x + y + myFunc2();
}

int main() {
    char8bits = 'a';
    int32bits = 32;
    int local32bits = 5;
    int funcRet     = myFunc();
    printf("string length 16");
}