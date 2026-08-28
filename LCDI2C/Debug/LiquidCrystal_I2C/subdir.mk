################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/LiquidCrystal_I2C/LCD_I2C.cpp 

OBJS += \
./LiquidCrystal_I2C/LCD_I2C.o 

CPP_DEPS += \
./LiquidCrystal_I2C/LCD_I2C.d 


# Each subdirectory must supply rules for building sources it contributes
LiquidCrystal_I2C/LCD_I2C.o: D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/LiquidCrystal_I2C/LCD_I2C.cpp LiquidCrystal_I2C/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"D:/my_Projects/Electronic/STM32/Keil/Libraries/DWT_Delay" -I"D:/my_Projects/Electronic/STM32/Keil/Libraries/LiquidCrystal_I2C" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-LiquidCrystal_I2C

clean-LiquidCrystal_I2C:
	-$(RM) ./LiquidCrystal_I2C/LCD_I2C.cyclo ./LiquidCrystal_I2C/LCD_I2C.d ./LiquidCrystal_I2C/LCD_I2C.o ./LiquidCrystal_I2C/LCD_I2C.su

.PHONY: clean-LiquidCrystal_I2C

