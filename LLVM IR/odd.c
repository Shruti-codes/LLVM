#include<stdio.h>

int main()
{
	int n,i,X=0;
	scanf("%d",&n);
	int A[n];
	for(i=0;i<n;i++)
		scanf("%d",&A[i]);

	for(i=0;i<n;i++)
		X = X^A[i];

	printf("%d \n", X);

}
