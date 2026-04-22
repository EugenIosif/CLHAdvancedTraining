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

typedef struct bitwiseOperationsCopy
{
    uint8_t value;
    uint8_t mask;
    uint8_t result;
} bitwiseOperationsCopy;

typedef struct PowerPoint
{
    uint16_t value;
    uint8_t mask;
    uint16_t result;
} PowerPoint;

BitwiseOperations golbalStruct = {0};

bitwiseOperationsCopy performBitwiseOperation(uint8_t value, uint8_t mask)
{
    bitwiseOperationsCopy operationResult;
    operationResult.value = value;
    operationResult.mask = mask;
    operationResult.result = value & mask; // Example bitwise AND operation

    return operationResult;
}

void copyResultToGlobalStruct(bitwiseOperationsCopy result)
{
    golbalStruct.value = result.value;
    golbalStruct.mask = result.mask;
    golbalStruct.result = result.result;
}

void copyResultToGlobalStructUsingPointer(uint8_t * result, uint8_t size)
{
    if(size != sizeof(BitwiseOperations))
    {
        printf("Size of result does not match size of global struct\n");
        return;
    }
    memcpy(&golbalStruct, result, size);    
}

void main(void)
{

    uint8_t value = 0b10101010; // Example value
    uint8_t mask = 0b11110000; // Example mask

    bitwiseOperationsCopy result = performBitwiseOperation(value, mask);

    //copying the result to global struct
    copyResultToGlobalStruct(result);
    printf("Value: %d, Mask: %d, Result: %d\n", golbalStruct.value, golbalStruct.mask, golbalStruct.result);
    
    memset(&golbalStruct, 0, sizeof(BitwiseOperations)); // Reset global struct
    
    //copying the result to global struct using pointer
    copyResultToGlobalStructUsingPointer((uint8_t*)&result, sizeof(BitwiseOperations));
    printf("Value: %d, Mask: %d, Result: %d\n", golbalStruct.value, golbalStruct.mask, golbalStruct.result);
}
