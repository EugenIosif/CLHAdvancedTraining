#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct bitwiseOperations
{
    uint8_t value;
    uint8_t mask;
    uint8_t result;
} BitwiseOperations;

char bitWiseArray[5][9] = {"", "", "", "", ""};

void main(void)
{
    BitwiseOperations array[5] = {
        {0b10101010, 0b00000000, 0},
        {0b11001100, 0b00000000, 0},
        {0b11110000, 0b00000000, 0},
        {0b00001111, 0b00000000, 0},
        {0b00110011, 0b00000000, 0}
    };

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            strncat(bitWiseArray[i], ((array[i].value & (1 << j)) ? "1" : "0"), 1);
        }
        strncat(bitWiseArray[i], "\0", 1);
    }

    for(int i = 0; i < 5; i++)
    {
        printf("Value: %s\n", bitWiseArray[i]);
    }


}
