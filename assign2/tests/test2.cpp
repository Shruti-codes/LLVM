int *a,*y,**x,q[9],b;
int main()
{
	*y=11;
	b = 21;
	
	x=&a;
	if(q[1] < 0)
	{
		**x = 11;
		q[b] =21;
	}
	y = *x;
	return 0;

}
