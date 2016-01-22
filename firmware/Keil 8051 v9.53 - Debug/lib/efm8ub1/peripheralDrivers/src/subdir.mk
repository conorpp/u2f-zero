################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/efm8ub1/peripheralDrivers/src/usb_0.c 

OBJS += \
./lib/efm8ub1/peripheralDrivers/src/usb_0.OBJ 


# Each subdirectory must supply rules for building sources it contributes
lib/efm8ub1/peripheralDrivers/src/%.OBJ: ../lib/efm8ub1/peripheralDrivers/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	wine /home/pp/SimplicityStudio_v3/developer/toolchains/keil_8051/9.53/BIN/C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

lib/efm8ub1/peripheralDrivers/src/usb_0.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h


