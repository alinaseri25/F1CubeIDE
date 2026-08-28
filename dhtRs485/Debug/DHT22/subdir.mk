################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DHT22/DHT22.cpp 

OBJS += \
./DHT22/DHT22.o 

CPP_DEPS += \
./DHT22/DHT22.d 


# Each subdirectory must supply rules for building sources it contributes
DHT22/DHT22.o: D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DHT22/DHT22.cpp DHT22/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=c++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/ArduinoJson/src" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DHT22" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DWT_Delay" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/jsonPacket" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-DHT22

clean-DHT22:
	-$(RM) ./DHT22/DHT22.cyclo ./DHT22/DHT22.d ./DHT22/DHT22.o ./DHT22/DHT22.su

.PHONY: clean-DHT22

