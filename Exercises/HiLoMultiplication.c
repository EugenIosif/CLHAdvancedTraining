#include <stdio.h>
#include <stdint.h>

typedef struct uint64Compound
{
    uint32_t high;
    uint32_t low;
}uint64Compound;

void multiplyUint64(uint64Compound *a, uint64Compound *b, uint64Compound *product);
uint64_t safe_mult(uint64_t a, uint64_t b);

uint32_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array) {
    uint32_t size = 0;
    while (number > 0) {
        *(array + size) = number % 10; // Store the least significant digit first
        number /= 10; // Remove the last digit
        size++;
    }
    return size; // Return the number of digits stored
}

uint64_t storeArrayInNumber(uint8_t* array, uint16_t size) {
    uint64_t number = 0;
    for (int i = size - 1; i >= 0; i--) {
        number = number * 10;
        number += *(array + i); // Reconstruct the number
    }
    return number;
}

uint64_t safe_mult(uint64_t a, uint64_t b)
{
    uint8_t arr1[50] = {0}, arr2[50] = {0}, ans[100] = {0};  // size of (arr1_size + arr2_size)

    uint8_t size1 = 0 , size2 = 0;

    size1 = storeIntInInverseOrderArray(a, &arr1[0]);
    size2 = storeIntInInverseOrderArray(b, &arr2[0]);

    for(int i=0;i<size1;i++)
    {
        for(int j=0;j<size2;j++)
        {
            ans[i+j] += arr1[i] * arr2[j];
            ans[i+j+1] += ans[i+j]/10;
            ans[i+j] %= 10;
        }
    }

    uint64_t product = 0;
    
    product = storeArrayInNumber(&ans[0], size1+size2);
    printf("Product: %llu\n", product);

    return product;
}

void multiplyUint64(uint64Compound *a, uint64Compound *b, uint64Compound *product)
{

    uint64_t a_upper = a->high;  // Upper 32 bits of 'a'
    uint64_t a_lower = a->low;  // Lower 32 bits of 'a'
    uint64_t b_upper = b->high;  // Upper 32 bits of 'b'
    uint64_t b_lower = b->low;  // Lower 32 bits of 'b'

    // Calculate the four partial products
    uint64_t p1 = safe_mult(a_upper, b_upper);
    uint64_t p2 = safe_mult(a_upper, b_lower);
    uint64_t p3 = safe_mult(a_lower, b_upper);
    uint64_t p4 = safe_mult(a_lower, b_lower);

    // Add the lower parts of p2 and p3 to get the low 32 bits of the result
    uint64_t lower = p4 + (p2 << 32) + (p3 << 32);

    // Calculate the carry from the lower to the upper part
    uint64_t carry = (lower < p4) ? 1 : 0;

    // Calculate the upper part of the result
    uint64_t upper = p1 + (p2 >> 32) + (p3 >> 32) + carry;

    // Set the result and overflow variables
    product->high = upper;
    product->low = lower;
}

void main(void)
{

    uint64Compound a = {4660, 22136};
    uint64Compound b = {39612, 57072};
    uint64Compound product = {0, 0};

    multiplyUint64(&a, &b, &product);

    printf("the product of the two compound numbers is: %d%d\n", product.high, product.low);
    //1846033481938545792
}