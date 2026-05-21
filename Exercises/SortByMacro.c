#include "stdio.h"
#include "stdint.h"
#include "string.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define GREATER(a, b) ((a) > (b))
#define LESSER(a, b) ((a) < (b))

#define SWAP_INCREASING(a, b)  {  \
                            if(GREATER(a, b)) { \
                                typeof(a) temp = a; \
                                a = b; \
                                b = temp; \
                            } \
                    }
#define SWAP_DECREASING(a, b)  {  \
                            if(LESSER(a, b)) { \
                                typeof(a) temp = a; \
                                a = b; \
                                b = temp; \
                            } \
                    }

uint16_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array);
void OrderDigitsInArray(uint8_t* array, uint16_t size);
void main(void);

void OrderDigitsInArray(uint8_t* array, uint16_t size)
{

    for(int i=0;i<size-1;i++)
    {
        for(int j=0;j<size-i-1;j++)
        {
            SWAP_DECREASING(array[j], array[j+1]);
        }
    }
}

uint16_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array) {
    uint16_t size = 0;
    printf("For number: %llu ", number);
    while (number > 0) {
        *(array + size) = number % 10; // Store the least significant digit first
        number /= 10; // Remove the last digit
        size++;
    }
    printf("Size of array: %d\n", size);
    return size; // Return the number of digits stored
}

void main(void) {
    uint64_t number = 2026;
    uint8_t array[20] = {0};
    uint16_t sizeOfArray = 0;


    sizeOfArray = storeIntInInverseOrderArray(number, array);

    printf("Original number: %llu\n", number);
    printf("Array of digits: ");
    for (int i = 0; i < sizeOfArray; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    OrderDigitsInArray(array, sizeOfArray);
    printf("Array of digits after sorting: ");
    for (int i = 0; i < sizeOfArray; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

}