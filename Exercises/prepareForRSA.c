#include <stdio.h>
#include <stdint.h>
#include <string.h>

void convertStringToDecimal(char* str, uint64_t* decimal);
void convertDecimalToString(char* str, uint64_t decimal);

void convertStringToDecimal(char* str, uint64_t* decimal) {
    uint8_t i = 0;
    while(str[i] != '\0') {
        *decimal = (*decimal << 8) + (uint8_t)str[i];
        i++;
    }
}

void reverseOrder(char* str, uint8_t size)
{
    char tempString[size];
    for(int i = 0; i < size; i++)
    {
        tempString[size-i-1] = str[i];
    }
    memcpy(str, tempString, size);
}

void convertDecimalToString(char* str, uint64_t decimal)
{
    uint8_t size = 0;
    while(decimal > 0)
    {
        *(str + size++) = (char)(decimal & 0xFF);
        decimal >>= 8;
    }
    *(str + size) = '\0';

    reverseOrder(str, size);
}

void stringFromEncryptedDecimal(uint64_t decimal, char *str, int size) {
    int index = 0;
    uint8_t localSize = size;
    while (decimal > 0 && index < size - 1) { // Leave space for null terminator
        str[index+localSize] = (char)((decimal & (0xFF << localSize))>>localSize); // Get the last 8 bits
        // decimal >>= 8; // Shift right by 8 bits to get the next character
        localSize--;
    }
    str[index] = '\0'; // Null-terminate the string
}


void main(void)
{
    char* str = "HELLO";
    // printf("\n\nString: %s\n", str);
    uint64_t value = 0;
    convertStringToDecimal(&str[0], &value);
    // printf("The returned value is: %llu", value);
    char returnString[10] = {0};
    // convertDecimalToString(returnString, value);
    // stringFromEncryptedDecimal(value, returnString, 9);
    // printf("\n\nString: %s\n", returnString);
    printf("Max int 16: %llu\n", UINT16_MAX);
    printf("0xFF << 8: %llu\n", 0xFF << 8 | 0xFF);
    printf("Max int 64: %llu\n", UINT64_MAX);
    printf("0xFF << 8: %llu\n", 0xFF << 25);

}