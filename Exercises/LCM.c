#include <stdio.h>
#include <stdint.h>

void main(void)
{
    uint8_t arrayA[10] = {0}, arrayB[10] = {0};
    uint8_t a = 16, b = 28;

    for(int i=0;i<10;i++)
    {
        arrayA[i] = a * (i+1);
        arrayB[i] = b * (i+1);
    }
    
    printf("Multiples of A: ");
    for(int i=0;i<10;i++)
    {
        printf("%d ", arrayA[i]);
    }
    printf("\n");

    printf("Multiples of B: ");
    for(int i=0;i<10;i++)
    {
        printf("%d ", arrayB[i]);
    }
    printf("\n");

    printf("Least common multiple of %d and %d is ", a, b);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            if(arrayA[i] == arrayB[j])
            {
                printf("%d\n\r", arrayA[i]);
                goto end;
            }
        }
    }
end:
}
