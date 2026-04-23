#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "math.h"

uint16_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array) {
    uint16_t size = 0;
    while (number > 0) {
        *(array + size) = number % 10; // Store the least significant digit first
        number /= 10; // Remove the last digit
        size++;
    }
    return size; // Return the number of digits stored
}

// long power(int x, unsigned n)
// {
//     // Initialize result to 1
//     long long pow = 1;

//     // Multiply x for n times
//     for (int i = 0; i < n; i++) {
//         pow = pow * x;
//     }

//     return pow;
// }

uint64_t storeArrayInNumber(uint8_t* array, uint16_t size) {

        uint64_t number = 0 ; 
        for ( int i =0 ; i < size ; i ++)
        {
         number = number + *(array+i) * pow(10,i) ; 
        // array ++ ;
        }
        return number ; 
}

void main(void) {
    uint64_t number = 2026;
    uint8_t array[20] = {0};
    uint16_t sizeOfArray = 0;
    uint64_t reconstructedNumber = 0;
    
    sizeOfArray = storeIntInInverseOrderArray(number, array);
    
    printf("Array in inverse order: ");
    for (int i = 0; i < sizeOfArray; i++) {
        printf("%d ", array[i]);
    }
    reconstructedNumber = storeArrayInNumber(array, sizeOfArray);
    printf("\nReconstructed number: %llu\n", reconstructedNumber);
}