################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/technicalDepartment/software/CubeIDE_Libraries/DWT_Delay/dwt_stm32_delay.c 

C_DEPS += \
./DWT_Delay/dwt_stm32_delay.d 

OBJS += \
./DWT_Delay/dwt_stm32_delay.o 


# Each subdirectory must supply rules for building sources it contributes
DWT_Delay/dwt_stm32_delay.o: D:/technicalDepartment/software/CubeIDE_Libraries/DWT_Delay/dwt_stm32_delay.c DWT_Delay/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"../../../CubeIDE_Libraries/DWT_Delay" -I"../../../CubeIDE_Libraries/LiquidCrystal_I2C" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-DWT_Delay

clean-DWT_Delay:
	-$(RM) ./DWT_Delay/dwt_stm32_delay.cyclo ./DWT_Delay/dwt_stm32_delay.d ./DWT_Delay/dwt_stm32_delay.o ./DWT_Delay/dwt_stm32_delay.su

.PHONY: clean-DWT_Delay

