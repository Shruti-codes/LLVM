//redundant load and stores
#include<stdio.h>
int add()
{
    int s;
    int a=30, b=20;
    s=a+b;
    int x=s;
    return x;
}