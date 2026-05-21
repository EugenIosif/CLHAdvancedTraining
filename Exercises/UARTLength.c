#include "stdio.h"

#include "stdint.h"

 

#define PARITY_NONE 0

#define PARITY_ODD 1

#define PARITY_EVEN 2

 

#define STOP_BIT_1 1

#define STOP_BIT_2 2

 

#define DATA_BIT_5 5

#define DATA_BIT_6 6

#define DATA_BIT_7 7

#define DATA_BIT_8 8

#define DATA_BIT_9 9

 

#define PARITY_BIT PARITY_NONE

#define STOP_BIT STOP_BIT_1

#define DATA_BIT_LENGTH  DATA_BIT_8

#define START_BIT 1

#define BUFFLENGTH (START_BIT + DATA_BIT_LENGTH + (PARITY_BIT != PARITY_NONE ? 1 : 0) + STOP_BIT)

 



 

void main(void)

{

 

    uint8_t buffer[BUFFLENGTH] = {0};

    for(uint8_t i = 0; i < BUFFLENGTH ; i++)

    {

        buffer[i] = 0xFF;
        
          printf("buffer[%d] = 0x%X\n", i, buffer[i]);
    }
  
}