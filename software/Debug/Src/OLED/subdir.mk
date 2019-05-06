################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/OLED/ssd1306.c \
../Src/OLED/ssd1306_fonts.c 

OBJS += \
./Src/OLED/ssd1306.o \
./Src/OLED/ssd1306_fonts.o 

C_DEPS += \
./Src/OLED/ssd1306.d \
./Src/OLED/ssd1306_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
Src/OLED/%.o: ../Src/OLED/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F446xx -I"C:/Users/luiza/F4/probazmenu/Inc" -I"C:/Users/luiza/F4/probazmenu/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/luiza/F4/probazmenu/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/luiza/F4/probazmenu/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/luiza/F4/probazmenu/Drivers/CMSIS/Include" -I"C:/Users/luiza/F4/probazmenu/Middlewares/Third_Party/FatFs/src"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


