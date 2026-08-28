################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DWT_Delay/dwt_stm32_delay.c 

C_DEPS += \
./DWT_Delay/dwt_stm32_delay.d 

OBJS += \
./DWT_Delay/dwt_stm32_delay.o 


# Each subdirectory must supply rules for building sources it contributes
DWT_Delay/dwt_stm32_delay.o: D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DWT_Delay/dwt_stm32_delay.c DWT_Delay/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F107xC -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/74HC595" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/MatrixKeyBoard" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DWT_Delay" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/SaveKey" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/LnkList" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/Needed" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-DWT_Delay

clean-DWT_Delay:
	-$(RM) ./DWT_Delay/dwt_stm32_delay.cyclo ./DWT_Delay/dwt_stm32_delay.d ./DWT_Delay/dwt_stm32_delay.o ./DWT_Delay/dwt_stm32_delay.su

.PHONY: clean-DWT_Delay

