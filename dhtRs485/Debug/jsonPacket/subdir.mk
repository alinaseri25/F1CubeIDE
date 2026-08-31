################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
D:/my_Projects/Electronic/STM32/F1/cubeIDE/CubeIDE_Libraries/jsonPacket/jsonpacket.cpp 

OBJS += \
./jsonPacket/jsonpacket.o 

CPP_DEPS += \
./jsonPacket/jsonpacket.d 


# Each subdirectory must supply rules for building sources it contributes
jsonPacket/jsonpacket.o: D:/my_Projects/Electronic/STM32/F1/cubeIDE/CubeIDE_Libraries/jsonPacket/jsonpacket.cpp jsonPacket/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=c++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../../../CubeIDE_Libraries/ArduinoJson/src -I../../../CubeIDE_Libraries/DHT22 -I../../../CubeIDE_Libraries/DWT_Delay -I../../../CubeIDE_Libraries/jsonPacket -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-jsonPacket

clean-jsonPacket:
	-$(RM) ./jsonPacket/jsonpacket.cyclo ./jsonPacket/jsonpacket.d ./jsonPacket/jsonpacket.o ./jsonPacket/jsonpacket.su

.PHONY: clean-jsonPacket

