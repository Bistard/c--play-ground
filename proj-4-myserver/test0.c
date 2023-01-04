#include <stdio.h>

int getNum(int num = 1000)
{
    return num;
}


int main()
{
    int a = 5;
    printf("%d\n", getNum());
}