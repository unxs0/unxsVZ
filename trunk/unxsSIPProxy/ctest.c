#include <stdio.h>
void ctest1(int *,char *cData);
void ctest2(int *,char *cData);

void ctest1(int *i,char *cData)
{
   *i=5;
}
            

void ctest2(int *i,char *cData)
{
   *i=100;
}
            

int main()
{
   int x;
   ctest1(&x,"test");
   printf("Valx=%d\n",x);

   return 0;
}
