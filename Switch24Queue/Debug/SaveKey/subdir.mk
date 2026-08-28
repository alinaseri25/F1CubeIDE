################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/SaveKey/SAVEKEY.cpp 

OBJS += \
./SaveKey/SAVEKEY.o 

CPP_DEPS += \
./SaveKey/SAVEKEY.d 


# Each subdirectory must supply rules for building sources it contributes
SaveKey/SAVEKEY.o: D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/SaveKey/SAVEKEY.cpp SaveKey/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F107xC -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/74HC595" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/MatrixKeyBoard" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DWT_Delay" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/SaveKey" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/LnkList" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/Needed" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-SaveKey

clean-SaveKey:
	-$(RM) ./SaveKey/SAVEKEY.cyclo ./SaveKey/SAVEKEY.d ./SaveKey/SAVEKEY.o ./SaveKey/SAVEKEY.su

.PHONY: clean-SaveKey

