#include <stdio.h>
#include <stdlib.h>

double dInnerProduct(const double *a, const double *b, int num)
{
	const double* end=a+num;
	double sum=0;

	while(a!=end)
	{
		sum+=(*a)*(*b);
		++a;
		++b;
	}
	return sum;
}

double dAccumulate(double v[], int n)
{
	double result = 0.0;
	register int i;
	for(i=0;i<n;i++)
		result+=v[i];
	return result;
}

double dDotProduct(double v[], double u[], int n)
{
    double result = 0.0;
    register int i;
    for (i = 0; i < n; i++)
        result += v[i]*u[i];
    return result;
}

main()
{
	double dXArray[7]={1,2,3,4,5,6,7};
	double dYArray[7]={167, 173, 186, 187, 201, 202, 210};

	double dX=dAccumulate(dXArray,7);
	double dY=dAccumulate(dYArray,7);
	double dProductXY=dInnerProduct(dXArray,dYArray,7);
	double dProductXX=dInnerProduct(dXArray,dXArray,7);

	double dSlope=(7*dProductXY - dX*dY) / (7*dProductXX - dX*dX);
	printf("dSlope=%lf dProductXY=%lf dProductXX=%lf dX=%lf dY=%lf\n",
			dSlope,dProductXY,dProductXX,dX,dY);
	double dAX=dX/7;
	double dAY=dY/7;
	double dIntercept= dAY - ( dSlope * dAX );
	printf("dIntercept=%lf\n",dIntercept);
}
