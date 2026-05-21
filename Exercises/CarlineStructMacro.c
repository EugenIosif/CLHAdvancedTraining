#include "stdio.h"
#include "stdint.h"
#include "string.h"

#define CARLINE_SUV 0
#define CARLINE_SEDAN 1
#define MODEL_YEAR_EARLIER_THAN_2019 0
#define MODEL_YEAR_LATER_THAN_2020 1

#define STARTBYTEPOSITION 10
#define AMMOUNTOFDATA_SEDAN 10
#define AMMOUNTOFDATA_SUV 15


#define CARLINE CARLINE_SUV
#define MODEL_YEAR MODEL_YEAR_EARLIER_THAN_2019

typedef struct CarSettings
{
    uint8_t variant[20];
    uint32_t model;
    uint32_t year;
    uint8_t color[3];
} CarSettings;

void printfunction(uint8_t *data)
{
    
}

void main(void)
{

#if CARLINE == CARLINE_SUV
    CarSettings suvCar;
    suvCar.model = 2020;
    suvCar.year = 2021;
    suvCar.color[0] = 0xFF;
    suvCar.color[1] = 0xFF;
    suvCar.color[2] = 0xFF;
#elif CARLINE == CARLINE_SEDAN
    CarSettings sedanCar;
    sedanCar.model = 2020;
    sedanCar.year = 2021;
    sedanCar.color[0] = 0xFF;
    sedanCar.color[1] = 0xFF;
    sedanCar.color[2] = 0xFF;
#endif

#if CARLINE == CARLINE_SUV
    printfunction(&suvCar.variant[0]);
#elif CARLINE == CARLINE_SEDAN
    printfunction(&sedanCar.variant[0]);
#endif

}
