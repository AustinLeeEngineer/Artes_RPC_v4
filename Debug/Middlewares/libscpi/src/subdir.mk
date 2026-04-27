################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/libscpi/src/error.c \
../Middlewares/libscpi/src/expression.c \
../Middlewares/libscpi/src/fifo.c \
../Middlewares/libscpi/src/ieee488.c \
../Middlewares/libscpi/src/lexer.c \
../Middlewares/libscpi/src/minimal.c \
../Middlewares/libscpi/src/parser.c \
../Middlewares/libscpi/src/units.c \
../Middlewares/libscpi/src/utils.c 

OBJS += \
./Middlewares/libscpi/src/error.o \
./Middlewares/libscpi/src/expression.o \
./Middlewares/libscpi/src/fifo.o \
./Middlewares/libscpi/src/ieee488.o \
./Middlewares/libscpi/src/lexer.o \
./Middlewares/libscpi/src/minimal.o \
./Middlewares/libscpi/src/parser.o \
./Middlewares/libscpi/src/units.o \
./Middlewares/libscpi/src/utils.o 

C_DEPS += \
./Middlewares/libscpi/src/error.d \
./Middlewares/libscpi/src/expression.d \
./Middlewares/libscpi/src/fifo.d \
./Middlewares/libscpi/src/ieee488.d \
./Middlewares/libscpi/src/lexer.d \
./Middlewares/libscpi/src/minimal.d \
./Middlewares/libscpi/src/parser.d \
./Middlewares/libscpi/src/units.d \
./Middlewares/libscpi/src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/libscpi/src/%.o Middlewares/libscpi/src/%.su Middlewares/libscpi/src/%.cyclo: ../Middlewares/libscpi/src/%.c Middlewares/libscpi/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L433xx -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I"C:/Task(local non-onedrive)/Artes_RPC_v3/libscpi/src" -I"C:/Task(local non-onedrive)/Artes_RPC_v3/libscpi/inc" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Core/ThreadSafe -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-libscpi-2f-src

clean-Middlewares-2f-libscpi-2f-src:
	-$(RM) ./Middlewares/libscpi/src/error.cyclo ./Middlewares/libscpi/src/error.d ./Middlewares/libscpi/src/error.o ./Middlewares/libscpi/src/error.su ./Middlewares/libscpi/src/expression.cyclo ./Middlewares/libscpi/src/expression.d ./Middlewares/libscpi/src/expression.o ./Middlewares/libscpi/src/expression.su ./Middlewares/libscpi/src/fifo.cyclo ./Middlewares/libscpi/src/fifo.d ./Middlewares/libscpi/src/fifo.o ./Middlewares/libscpi/src/fifo.su ./Middlewares/libscpi/src/ieee488.cyclo ./Middlewares/libscpi/src/ieee488.d ./Middlewares/libscpi/src/ieee488.o ./Middlewares/libscpi/src/ieee488.su ./Middlewares/libscpi/src/lexer.cyclo ./Middlewares/libscpi/src/lexer.d ./Middlewares/libscpi/src/lexer.o ./Middlewares/libscpi/src/lexer.su ./Middlewares/libscpi/src/minimal.cyclo ./Middlewares/libscpi/src/minimal.d ./Middlewares/libscpi/src/minimal.o ./Middlewares/libscpi/src/minimal.su ./Middlewares/libscpi/src/parser.cyclo ./Middlewares/libscpi/src/parser.d ./Middlewares/libscpi/src/parser.o ./Middlewares/libscpi/src/parser.su ./Middlewares/libscpi/src/units.cyclo ./Middlewares/libscpi/src/units.d ./Middlewares/libscpi/src/units.o ./Middlewares/libscpi/src/units.su ./Middlewares/libscpi/src/utils.cyclo ./Middlewares/libscpi/src/utils.d ./Middlewares/libscpi/src/utils.o ./Middlewares/libscpi/src/utils.su

.PHONY: clean-Middlewares-2f-libscpi-2f-src

