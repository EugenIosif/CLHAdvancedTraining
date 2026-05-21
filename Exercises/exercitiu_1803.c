
#include <stdio.h>

#define MYVALUE 0x2F

#define MAX(a,b) ((a)>(b)?(a):(b))

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int main()
{
   unsigned char val = MYVALUE; //0010 1111 

   printf("max(3,5) = %d\n", MAX(3,5));
    return 0;

}