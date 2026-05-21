################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/wolfssl_example.c 

OBJS += \
./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/wolfssl_example.o 

C_DEPS += \
./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/wolfssl_example.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/%.o Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/%.su Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/%.cyclo: ../Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/%.c Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32U5xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../wolfSSL -I../Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/ -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Third_Party-2f-wolfSSL_wolfSSL_wolfSSL-2f-wolfssl-2f-IDE-2f-STM32Cube

clean-Middlewares-2f-Third_Party-2f-wolfSSL_wolfSSL_wolfSSL-2f-wolfssl-2f-IDE-2f-STM32Cube:
	-$(RM) ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/wolfssl_example.cyclo ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/wolfssl_example.d ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/wolfssl_example.o ./Middlewares/Third_Party/wolfSSL_wolfSSL_wolfSSL/wolfssl/IDE/STM32Cube/wolfssl_example.su

.PHONY: clean-Middlewares-2f-Third_Party-2f-wolfSSL_wolfSSL_wolfSSL-2f-wolfssl-2f-IDE-2f-STM32Cube

