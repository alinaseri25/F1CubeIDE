################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f107rctx.s 

S_DEPS += \
./Core/Startup/startup_stm32f107rctx.d 

OBJS += \
./Core/Startup/startup_stm32f107rctx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m3 -g3 -DDEBUG -c -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/74HC595" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/MatrixKeyBoard" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/DWT_Delay" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/SaveKey" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/LnkList" -I"D:/my_Projects/Electronic/STM32/F1/CubeIDE/Libraries/Needed" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f107rctx.d ./Core/Startup/startup_stm32f107rctx.o

.PHONY: clean-Core-2f-Startup

