#include<stdio.h>
//global merging in -O2

static int arr1[10], arr2[10], arr3[10];
int main()
{
for (int i = 0; i<10;i++) 
{
   arr1[i] = arr2[i]*arr3[i];
}
return arr1[9];
}
