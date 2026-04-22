#include <stdio.h>
#include <stdint.h>

uint8_t array_global[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

uint8_t find(uint8_t * a, uint8_t * b)
{
    uint8_t returnValue = 0xFF;
    for(int i = 0; i < 8; i++)
    {
        if( (*(a+i) == *(b+0)) && 
            (*(a+i+1) == *(b+1)) && 
            (*(a+i+2) == *(b+2))
          )
        {
            returnValue = i;
            break;
        }
    }
    return returnValue;
}

void main(void)
{
    uint8_t array_local[3][3] = {
                                    {1, 2, 3},
                                    {4, 6, 7},
                                    {5, 6, 7}
    };

    for(int i = 0; i < 3; i++)
    {
        printf("pozitia pe care se gaseste subsetul %d este: %d\n", i, find(&array_global[0], &array_local[i][0]));
    }
    
}