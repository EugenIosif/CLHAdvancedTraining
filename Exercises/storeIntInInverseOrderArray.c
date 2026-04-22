#include "stdio.h"
#include "stdint.h"
#include "string.h"

#define simple_rsa_decrypt simple_rsa_encrypt

uint16_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array);
uint64_t storeArrayInNumber(uint8_t* array, uint16_t size);
uint64_t safe_mult(uint64_t a, uint64_t b);
uint64_t safe_mult_mod(uint64_t a, uint64_t b, uint64_t n);

uint16_t storeIntInInverseOrderArray(uint64_t number, uint8_t* array) {
    uint16_t size = 0;
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
    uint8_t arr1[20] = {0}, arr2[20] = {0}, ans[40] = {0};  // size of (arr1_size + arr2_size)

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

    for(int i = 0; i < 40; i++)
    {
        printf("%d", ans[i]);
    }
    printf("\n");


    uint64_t product = 0;
    product = storeArrayInNumber(&ans[0], size1+size2);
    printf("Product: %llu\n", product);

    return product;
}

uint64_t safe_mult_mod(uint64_t a, uint64_t b, uint64_t n)
{
    return safe_mult(a, b) % n;
}

uint64_t simple_rsa_encrypt(uint64_t message, uint64_t e, uint64_t n)
{
    uint64_t result = 1;
    message = message % n;
    while (e > 0) {
        if (e % 2 == 1){
            result = safe_mult_mod(result, message, n);
        }
        e = e >> 1;
        message = safe_mult_mod(message, message, n);
    }
    return result;
}

uint64_t concatenateBitRepresentationIntoDecimal(char *str) {
    uint64_t returnValue = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        returnValue = (returnValue << 8) + (uint8_t)str[i];
    }
    return returnValue;
}

void stringFromEncryptedDecimal(uint64_t decimal, char *str, int size) {
    int index = 0;
    while (decimal > 0 && index < size - 1) { // Leave space for null terminator
        str[index++] = (char)(decimal & 0xFF); // Get the last 8 bits
        decimal >>= 8; // Shift right by 8 bits to get the next character
    }
    str[index] = '\0'; // Null-terminate the string
}


void main(void) {

    // char *str = "HELLO";
    // printf("\n\nString: %s\n", str);

    // uint64_t decimalRepresentation = concatenateBitRepresentationIntoDecimal(str);
    // printf("Decimal representation: %llu\n", decimalRepresentation);

        printf("the product between %llu and %llu is %llu\n\r", 1230987512309842,9243723401234412, safe_mult(1230987512309842, 9243723401234412));

    // uint64_t encryptedUint64 = simple_rsa_encrypt(decimalRepresentation, 65537, 493071499771);
    // printf("Encrypted uint64: %llu\n", encryptedUint64);

    // uint64_t decryptedUint64 = simple_rsa_decrypt(encryptedUint64, 266513779457, 493071499771);
    // printf("Decrypted uint64: %llu\n", decryptedUint64);

    // char decrypted_str[9];
    // stringFromEncryptedDecimal(decryptedUint64, &decrypted_str[0], (sizeof(decrypted_str)/sizeof(decrypted_str[0])));
    // printf("Decrypted string: %s\n", decrypted_str);

}