#include <stdio.h>

int main()
{
    int a = 0x80;
    int b = 0;

    for (int i = 0; i < 8; i++)
    {
        b |= ((a >> i) & 1) << (7 - i);
    }
    printf("a=0x%x b=0x%x\n", a, b);
    return 0;
}
