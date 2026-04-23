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
    uint8_t signature;
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

uint8_t getSignature(things * thing)
{
    if(thing != NULL)
    {
        return thing->signature;
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

void setSignature(things * thing, uint8_t signature)
{
    if(thing != NULL)
    {
        thing->signature = signature;
    }
}

void computeValue(things * thing)
{
uint64_t result = 0;
for (int i = 0; i<8 ; ++i)
{
    uint64_t byte = (getValue(thing) >> (i * 8)) & 0xFF; 
    uint8_t mask = getMask (thing, i);
    OperationType op = getOperation(thing, i);
    uint64_t computed;
    switch (op )
    {
        case AND: computed = byte & mask; break;
        case OR: computed = byte | mask; break;
        case NOT: computed = ~byte; break;
        case XOR: computed = byte ^ mask; break;
    }
    uint64_t tempmask = 0xFFULL << (i * 8);
    result &= ~tempmask;
    result |= (computed & 0xFFULL) << (i * 8);
}
    setValue(thing, result);
}

uint64_t testFunction(uint64_t * input, uint8_t newValue, uint8_t position)
{
    uint64_t result = *input;
    // printf("Original value: 0x%llX\n", result);
    uint64_t tempmask = (0xFFULL << (position * 8));
    // printf("temp mask: 0x%llX\n", tempmask);
    result &= ~tempmask;
    // printf("Result after masking: 0x%llX\n", result);

    result |= (newValue & 0xFFULL) << (position * 8);
    // printf("Result after setting new value: 0x%llX\n", result);
    return result;
}

uint8_t computeSignature (uint8_t * bytes)
{
    if(bytes != NULL)
    {
        uint8_t result = 0;
        for(int i = 0; i < 8; ++i)
        {
            // result ^= *(bytes + i); //Equivalent

            result ^= bytes[i]; //Equivalent

            // result ^= *bytes; //Equivalent
            // bytes++;
        }
        return result;
    }    
}

void flipBits(uint8_t *data, size_t len)
{
    if(data!= NULL && len > 0)
    {
        for (size_t i = 0; i < len; i++)
        {
            data[i] ^= 0xFF; // Flip all bits in the byte
        }
    }    
}

void prepareTransmission(things * thing, uint8_t * transmissionBuffer)
{
    if(thing != NULL)
    {
        memcpy(transmissionBuffer, &thing->signature, sizeof(thing->signature));
        memcpy(transmissionBuffer + sizeof(thing->signature), &thing->value, sizeof(thing->value));
        memset(transmissionBuffer + sizeof(thing->value) + sizeof(thing->signature), '\0', 1);
    }
}

void main(void)
{
    things arrayOfThings[5]= {{     {35, 132, 108, 174, 144, 241, 187, 235}, 
                                    {2, 1, 2, 2, 1, 1, 1, 3}, 
                                    41, 
                                    0
                                },
                                {
                                    {219, 135, 62, 36, 13, 6, 71, 179},
                                    {0, 0, 1, 2, 0, 3, 2, 2}, 
                                    11942, 
                                    0
                                }, 
                                {
                                    {200, 187, 166, 3, 125, 56, 31, 212},
                                    {3, 3, 3, 2, 1, 1, 1, 3},
                                    1869, 
                                    0
                                }, 
                                {
                                    {150, 69, 19, 137, 28, 174, 32, 80},
                                    {1, 3, 1, 2, 3, 2, 2, 2}, 
                                    27644, 
                                    0
                                },
                                {
                                    {173, 240, 149, 63, 177, 122, 244, 229},
                                    {0, 2, 1, 3, 0, 1, 2, 3}, 
                                    123456789, 
                                    0
                            }};

    uint8_t signatureArray[5] = {0};
    uint8_t transmissionBuffer[10];
    for(int i = 0; i < 5; ++i)
    {
        computeValue(&arrayOfThings[i]);
    }
    
    for(int i = 0; i < 5; ++i)
    {
        ValueUnion valueUnion;
        valueUnion.value = getValue(&arrayOfThings[i]);
        setSignature(&arrayOfThings[i], computeSignature(valueUnion.bytes));
    }
    //encrypt the signatures of the first 4 things
    for(int i = 0; i < 5; ++i)
    {
        flipBits(&arrayOfThings[i].signature, sizeof(arrayOfThings[i].signature));
    }
    prepareTransmission(&arrayOfThings[0], &transmissionBuffer[0]);
    printf("Transmission buffer for the first entry: ");

    for(int i = 0; i < 10; ++i)
    {
        printf("%02X ", transmissionBuffer[i]);
    }

    
    // uint64_t randomValue = 0xDEADBEEFCAFEBABE;
    // randomValue = testFunction(&randomValue, 0xE0, 4);
    // printf("Modified value: 0x%llX\n", randomValue);
}