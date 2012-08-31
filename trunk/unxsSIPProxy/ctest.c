#include <stdio.h>
void ctest1(int *);
void ctest2(int *);

void ctest1(int *i)
{
   *i=5;
}
            

void ctest2(int *i)
{
   *i=100;
}
            

int main()
{
   int x;
   ctest1(&x);
   printf("Valx=%d\n",x);

   return 0;
}
