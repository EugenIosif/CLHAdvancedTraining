#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef union {
    uint64_t value;
    uint8_t bytes[8];
} ValueUnion;

typedef enum OperationType {
    AND, 
    OR, 
    NOT,
    XOR
}OperationType;

typedef struct things
{
    uint8_t mask[8];
    OperationType OP[8];
    uint64_t value;
} things;

uint8_t getMask(things * thing, uint8_t position)
{
    if(thing != NULL && position < 8)
    {
        return thing->mask[position];
    }
    return 0;
}

OperationType getOperation(things * thing, uint8_t position)
{
    if(thing != NULL && position < 8)
    {
        return thing->OP[position];
    }
    return 0;
}

uint64_t getValue(things * thing)
{
    if(thing != NULL)
    {
        return thing->value;
    }
    return 0;
}

void setMask(things * thing, uint8_t position, uint8_t mask)
{
    if(thing != NULL && position < 8)
    {
        thing->mask[position] = mask;
    }
}

void setOperation(things * thing, uint8_t position, OperationType OP)
{
    if(thing != NULL && position < 8)
    {
        if(OP > 3)
        {
            printf("Invalid operation type");
            return;
        }
        thing->OP[position] = OP;
    }
}

void setValue(things * thing, uint64_t value)
{
    if(thing != NULL)
    {
        thing->value = value;
    }
}

void computeValue(things * thing)
{
uint64_t result = 0;
for (int i = 0; i<8 ; ++i)
{
    uint64_t byte = (getValue(thing) >> (i * 8)) & 0xFF; 
    uint64_t mask = getMask (thing, i);
    OperationType op = getOperation(thing, i);
    uint64_t computed;
    switch (op )
    {
        case AND: computed = byte & mask; break;
        case OR: computed = byte | mask; break;
        case NOT: computed = ~byte; break;
        case XOR: computed = byte ^ mask; break;
    }
    uint64_t tempmask = 0xFF << (1 * 8);
    computed &= ~tempmask;
    result |= (computed & 0xFF) << (i * 8);
}
    setValue(thing, result);
}

uint8_t computeRealValue (uint8_t * bytes)
{
    uint8_t result = 0;

    for(int i = 0; i < 8; ++i)
    {
        result ^= *bytes;
        bytes++;
    }

    //xor this byte to the result 
    return result;
}

void main(void)
{
    things arrayOfThings[5]= {{     {35, 132, 108, 174, 144, 241, 187, 235}, 
                                    {2, 1, 2, 2, 1, 1, 1, 3}, 
                                    41
                                },
                                {
                                    {219, 135, 62, 36, 13, 6, 71, 179},
                                    {0, 0, 1, 2, 0, 3, 2, 2}, 
                                    11942
                                }, 
                                {
                                    {200, 187, 166, 3, 125, 56, 31, 212},
                                    {3, 3, 3, 2, 1, 1, 1, 3},
                                    1869
                                }, 
                                {
                                    {150, 69, 19, 137, 28, 174, 32, 80},
                                    {1, 3, 1, 2, 3, 2, 2, 2}, 
                                    27644
                                },
                                {
                                    {173, 240, 149, 63, 177, 122, 244, 229},
                                    {0, 2, 1, 3, 0, 1, 2, 3}, 
                                    123456789
                            }};

    uint8_t realValues[5] = {0};
    for(int i = 0; i < 5; ++i)
    {
        computeValue(&arrayOfThings[i]);
    }
    
    for(int i = 0; i < 5; ++i)
    {
        ValueUnion valueUnion;
        valueUnion.value = getValue(&arrayOfThings[i]);
        realValues[i] = computeRealValue(valueUnion.bytes);
    }

    for(int i = 0; i < 5; ++i)
    {
        printf("Real value %d: %d\n", i, realValues[i]);
    }

}