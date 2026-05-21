#include "stdio.h"
#include "stdint.h"

#include "myMemoryHeader.h"

void processBuffer(uint8_t command);

void processBuffer(uint8_t command)
{
    uint8_t option1 = (command & OPTION1_MASK) >> OPTION1_POSITION;
    uint8_t option2 = (command & OPTION2_MASK) >> OPTION2_POSITION;
    uint8_t option3 = (command & OPTION3_MASK) >> OPTION3_POSITION;
    uint8_t option4 = (command & OPTION4_MASK) >> OPTION4_POSITION;

    printf("Option 1: %d\n", option1);
    printf("Option 2: %d\n", option2);
    printf("Option 3: %d\n", option3);
    printf("Option 4: %d\n", option4);
}

void main(void)
{
    uint8_t buffer[BUFFLENGTH] = {0};
    uint8_t command = 0b10010101; // Example command with various options set

    processBuffer(command);
}