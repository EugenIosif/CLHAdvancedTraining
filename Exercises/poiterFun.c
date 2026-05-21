#include <stdio.h>
#include <stdint.h>

void printByParameter(int par)
{
    printf("The value passed by integer is %d", par);
}

void printByRefference(int * p)
{
    printf("The value passed by refference is %d\n", *p);
}

void printByPointer(int * p)
{
    printf("The value passed by pointer is %d\n", *p);
}

void main(void)
{    
    int a = 0xABCD;
    printByRefference(&a);
    // int *ptr = &a;
    // int& ref = a;

    // printf("Value of a: 0x%X\n", a);
    // printf("Address of a: 0x%X\n", &a);
    // printf("Value of ptr: 0x%X\n", ptr);
    // printf("Address of ptr: 0x%X\n", &ptr);
    // printf("Value pointed to by ptr: 0x%X\n", *ptr);

    // printByPointer(ptr);
}

