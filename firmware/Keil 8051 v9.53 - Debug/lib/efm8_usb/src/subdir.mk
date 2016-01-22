################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/efm8_usb/src/efm8_usbd.c \
../lib/efm8_usb/src/efm8_usbdch9.c \
../lib/efm8_usb/src/efm8_usbdep.c \
../lib/efm8_usb/src/efm8_usbdint.c 

OBJS += \
./lib/efm8_usb/src/efm8_usbd.OBJ \
./lib/efm8_usb/src/efm8_usbdch9.OBJ \
./lib/efm8_usb/src/efm8_usbdep.OBJ \
./lib/efm8_usb/src/efm8_usbdint.OBJ 


# Each subdirectory must supply rules for building sources it contributes
lib/efm8_usb/src/%.OBJ: ../lib/efm8_usb/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	wine /home/pp/SimplicityStudio_v3/developer/toolchains/keil_8051/9.53/BIN/C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

lib/efm8_usb/src/efm8_usbd.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_assert/assert.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h

lib/efm8_usb/src/efm8_usbdch9.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h

lib/efm8_usb/src/efm8_usbdep.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h

lib/efm8_usb/src/efm8_usbdint.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/u2f-firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h


