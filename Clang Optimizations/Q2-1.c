#include<stdio.h>

//effect of inlining in -O2

inline int call2()
{
	int x=2, y=5;
	return (x+y);
}

inline int call1()
{
	return call2();
}

int main()
{
	printf("%d",call1());
	return 0;
}