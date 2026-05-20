################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stm32.c \
../Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stsafe.c 

OBJS += \
./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stm32.o \
./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stsafe.o 

C_DEPS += \
./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stm32.d \
./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stsafe.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/%.o Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/%.su Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/%.cyclo: ../Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/%.c Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32U5xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../wolfSSL -I../Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-wolfSSL_wolfSSL_wolfSSL-2f-wolfssl-2f-wolfcrypt-2f-src-2f-port-2f-st

clean-Middlewares-2f-Third_Party-2f-wolfSSL_wolfSSL_wolfSSL-2f-wolfssl-2f-wolfcrypt-2f-src-2f-port-2f-st:
	-$(RM) ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stm32.cyclo ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stm32.d ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stm32.o ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stm32.su ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stsafe.cyclo ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stsafe.d ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stsafe.o ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/wolfcrypt/src/port/st/stsafe.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-wolfSSL_wolfSSL_wolfSSL-2f-wolfssl-2f-wolfcrypt-2f-src-2f-port-2f-st

