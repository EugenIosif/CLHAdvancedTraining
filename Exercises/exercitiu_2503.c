#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define OFFSET_FOR_SECOND_ARRAY 26

void shiftCipherLetters(char* lettersArray,int key);
char cesarCipherEncrypt(char* lettersArray,char ch);

void shiftCipherLetters(char* lettersArray,int key)
{
    //shift the letters in the second array by the key value - left shift by key possitions
    for(int i = 0; i < key; i++) { //repeat the shift process key number of times
        char temp = *(lettersArray + OFFSET_FOR_SECOND_ARRAY); //store the first letter of the second array before shifting == letters[1][0]
        for(int j = 0; j < 25; j++) {
            *(lettersArray + OFFSET_FOR_SECOND_ARRAY+j) = *(lettersArray + OFFSET_FOR_SECOND_ARRAY+j+1); //shift left by one position
        }
        *(lettersArray + OFFSET_FOR_SECOND_ARRAY + 25) = temp; //wrap around the first letter to the end of the array
    }
}

char cesarCipherEncrypt(char* lettersArray, char ch) 
{
    //lookup table for the current character
    for (int i = 0; i  <  26; i++) {
        if (ch == *(lettersArray+i)) {
            return *(lettersArray + OFFSET_FOR_SECOND_ARRAY+i); //return the corresponding letter from the second array
        }
    }
    return ch; //if the character is not found in the first array, return it unchanged
}

void main(void)
{
    char letters[2][26] =   {
                                {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
                                {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}
                            };
    char securePassword[15] = "SecurePass123";
    int key = 3; // Example key for Caesar cipher
    char originalString[] = "cybersecurity";
    char encryptedString[sizeof(originalString)] = {0};


    // printf("Original String: %s\n", originalString);
    shiftCipherLetters(&(letters[0][0]), key);

    // for(int i = 27; i< 100; i++)
    // {
    //     char test = letters[0][25+i]; //try to access a character from the second array by out of bounds access to verify if possible
    //     printf("Test character: %c\n", test);
    // }

            printf("Test character: %c\n", letters[5][0]);


    uint8_t counter = 0;
    while (originalString[counter] != '\0') {
        encryptedString[counter] = cesarCipherEncrypt(&(letters[0][0]), originalString[counter]);
        counter++;
    }
    // printf("Encrypted String: %s\n", encryptedString);

    printf("\n");
}