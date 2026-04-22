#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

bool checkParity(uint16_t value);
bool flipBitOnPosition(uint16_t *value, uint8_t position);

bool flipBitOnPosition(uint16_t *value, uint8_t position) {

    
    if (position < 16) {
        *value ^= (1 << position);
        // return *value;
        // printf("parameters are: %d and 0x%04X\n", position, *value);
        return true;
    }
    else { 
        // printf("\n\n\n\nparameters are: %d and 0x%04X\n", position, *value);

        printf("error: position must be between 0 and 15\n");
        return 0;
    }
}

bool checkParity(uint16_t value) {
    return ((value & 0x0001) == 0); // Check if the least significant bit is 0 (even parity)
}

void main(void)
{
    static uint16_t data[4] = {0x1235, 0x5678, 0x9ABD, 0xDEF0};
    uint8_t positionToFlip[4] = {3, 7, 11, 20}; // Example bit positions to flip (0-15)

    printf("Size of data: %lu bytes\n", (unsigned long)sizeof(data));

    // for(uint8_t i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
    //     if(checkParity(data[i])) {
    //         printf("Value: 0x%04X has even parity\n", data[i]);
    //     } else {
    //         printf("Value: 0x%04X has odd parity\n", data[i]);
    //     }
    // }

    // for(int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
    //     printf("Value before flipping bit at position %d: 0x%04X\n", positionToFlip[i], data[i]);
    // }

    for(uint8_t i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
        if(flipBitOnPosition(&data[i], positionToFlip[i])) {
            printf("Value after flipping bit at position %d: 0x%04X\n", positionToFlip[i], data[i]);
        } else {
            printf("Failed to flip bit at position %d for value 0x%04X\n", positionToFlip[i], data[i]);
        }
    }

    // for(int i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
    //     printf("Value before flipping bit at position %d: 0x%04X\n", positionToFlip[i], data[i]);
    // }

}