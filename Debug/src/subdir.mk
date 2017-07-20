################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ErrorTODO.cpp \
../src/Serial.cpp \
../src/SerialMon.cpp 

OBJS += \
./src/ErrorTODO.o \
./src/Serial.o \
./src/SerialMon.o 

CPP_DEPS += \
./src/ErrorTODO.d \
./src/Serial.d \
./src/SerialMon.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


