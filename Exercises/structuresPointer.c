#include <stdio.h>
#include <stdint.h>

#pragma scalar_storage_order big-endian

typedef union {
    uint16_t u16_Value;
    struct {
        uint8_t LSB;
        uint8_t MSB;
    }__attribute__((packed)) Bytes;
} u16_Union;

typedef struct {
    uint8_t u8_Value;
    uint16_t u16_Value;
    uint8_t u8_Array[8];
} Structure;

uint8_t findPositionOfU8(Structure* s, uint8_t u8_Value);
uint8_t findPositionOfU16(Structure* s, uint16_t u16_Value);

uint8_t findPositionOfU8(Structure* s, uint8_t u8_Value) {
    uint8_t returnValue = 0xFF;
    for(int i = 0; i < 8; i++)
    {
        if(*(s->u8_Array+i) == u8_Value)
        {
            returnValue = i;
            break;
        }
    }
    return returnValue;
}

uint8_t findPositionOfU16(Structure* s, uint16_t u16_inputValue) {
    uint8_t returnValue = 0xFF;
    u16_Union tempUnion;
    tempUnion.u16_Value = u16_inputValue;

    for(int i = 0; i < 7; i++)
    {
        if( (*(s->u8_Array+i) == tempUnion.Bytes.LSB) && 
            (*(s->u8_Array+i+1) == tempUnion.Bytes.MSB)
          )
        {
            returnValue = i;
            break;
        }
        
    }
    return returnValue;
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