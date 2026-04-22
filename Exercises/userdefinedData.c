#include <stdio.h>
#include <stdint.h>

struct Student {
    char name[50];
    int age;
    float grade;
};

struct OptimalStudent {
    int age;
    float grade;
    char name[50];
};

union Data {
    int i;
    float f;
    char str[20];
}; // This union will only be 20 bytes large.

typedef enum Level {
    val33, 
    val22,
    val11,
    val0
} Level; // This enum will typically be 4 bytes large, depending on the compiler.

// enum Level myVar = MEDIUM;

typedef union reg
{
    unsigned int value;
    struct {
        unsigned int p0 : 1; // bit 0
        unsigned int p1 : 1; // bit 1
        unsigned int p2 : 1; // bit 2
        unsigned int p3 : 1; // bit 3
        unsigned int p4 : 1; // bit 4
        unsigned int p5 : 1; // bit 5
        unsigned int p6 : 1; // bit 6
        unsigned int p7 : 1; // bit 7
    } pins;
} reg;


void main(void)
{
    reg PORTA;
    PORTA.value = 0b10101000;

    PORTA.pins.p3 = 1; // Set bit 3



    struct Student str = {.age = 20, .name = "John Doe", .grade = 3.5};

    // printf("the size of the struct is: %lu\n", sizeof(struct Student));
    // printf("the size of the optimal struct is: %lu\n", sizeof(struct Student));
    // printf("the size of the reg is: %lu\n", sizeof(union reg));
    // printf("the value of myVar is: %d\n", myVar);
    // printf("the size of unsigned int is %lu\n", sizeof(unsigned int));

    // union reg myReg;
    // myReg.a = 0xAAAAAAAA; // Example

    // uint32_t mask = 0x00000001; // Mask for bit 0
    // mask <<= 16; // Shift mask to bit 16
    // if(mask&myReg.a) {
    //     printf("Bit 16 is set.\n");
    // } else {
    //     printf("Bit 16 is not set.\n");
    // }

    // if(myReg.bits.b16) {
    //     printf("Bit 16 is set.\n");
    // } else {
    //     printf("Bit 16 is not set.\n");
    // }

    Level myLevel = val0;
    printf("The value of myLevel is: %d\n", myLevel);
    // myLevel = MEDIUM1;
    // printf("The value of myLevel is: %d\n", myLevel);
    // myLevel = MEDIUM2;
    // printf("The value of myLevel is: %d\n", myLevel);
    // myLevel = HIGH;
    // printf("The value of myLevel is: %d\n", myLevel);
    printf("The size of enum Level is: %lu\n", sizeof(enum Level));
}