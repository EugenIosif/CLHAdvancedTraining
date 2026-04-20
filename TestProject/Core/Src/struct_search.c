/*Sa se implementeze urmatoarea problema: Se primeste ca parametru transmis prin pointer o variabila 
de tip structura, care contine un uint8, un uint16 si un array de 8 bytes. 
Scopul este ca sa se caute in array-ul din structura pozitia variabilei de tip uint8 si pozitia variabilei uint16.
Daca nu se gaseste, se va returna 0xFF in ambele cazuri

Notes: se vor implementa doua functii, una pentru fiecare tip de data.
*/

#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t u8_Value;
    uint16_t u16_Value;
    uint8_t u8_Array[8];
} Structure;

uint8_t findPositionOfU8(Structure* s, uint8_t u8_Value);
uint8_t findPositionOfU16(Structure* s, uint16_t u16_Value);

uint8_t findPositionOfU8(Structure* s, uint8_t u8_Value) 
{
    for(uint8_t i = 0; i < 8; i++) {
        if(s->u8_Array[i] == u8_Value) {
            return i;
        }
    }
    return 0xFF; // Not found
}

uint8_t findPositionOfU16(Structure* s, uint16_t u16_Value) 
{
    for(uint8_t i = 0; i < 8; i+=2) {
        if(((uint16_t)s->u8_Array[i] << 8) + s->u8_Array[i + 1] == u16_Value) {
            return i;
        }
    }
    return 0xFF; // Not found
}

void main(void){
    uint8_t a = 0x12;
    uint16_t b = 0x3456;
    Structure myStruct = {0x12, 0x3456, {0x00, 0x01, 0x12, 0x35, 0x34, 0x56, 0x78, 0x9A}};

    uint8_t u8pos = findPositionOfU8(&myStruct, a);
    uint8_t u16pos = findPositionOfU16(&myStruct, b);
    //positia u8  =  2
    //pozitia u16 =  4
    printf("Position of u8 value: %d\n", u8pos);
    printf("Position of u16 value: %d\n", u16pos);
}