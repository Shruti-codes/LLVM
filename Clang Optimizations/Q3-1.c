#include<stdio.h>
long factorial(long n)
{
  if(n == 0)
    return 1;
  else
    return factorial(n - 1) * n;
}


int main()
{
  factorial(1000000000);
}