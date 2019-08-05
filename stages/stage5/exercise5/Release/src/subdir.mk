################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c 

OBJS += \
./src/main.o 

C_DEPS += \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DUSE_HAL_DRIVER -DSTM32F429xx -DUSE_FREERTOS_SYSTICK -DSTM32F4 -DSTM32F429ZITx -DNUCLEO_F429ZI -I"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/sourcelib/boardlib/Board" -I"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/freertoslib/config" -I"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/freertoslib/FreeRTOS/Source/include" -I"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/freertoslib/FreeRTOS/Source/Portable" -I"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/sourcelib/boardlib/CMSIS/core" -I"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/sourcelib/boardlib/CMSIS/device" -I"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/sourcelib/boardlib/HAL_Driver/Inc" -include"C:/Users/Kevin/Desktop/Kevin UQ/csse3010/CSSE3010-s4408736/sourcelib/boardlib/Board/stm32f4xx_it.c" -O3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


