#include<stdio.h>

int main()
{
	int n,i,max,flag=0,res;
	scanf("%d",&n);
	int A[n],C[n];
	for(i=0; i<n; i++)
	{
		scanf("%d", &A[i]);
		C[i] = 0;
	}

	max = C[0];
	for(i=0; i<n; i++)
		C[A[i]] += 1;

	for(i=0;i<n;i++)
		if(C[i] > max)
		{
			max = C[i];
			res = i;
		}
		printf("%d",res);
}