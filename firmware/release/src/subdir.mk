################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
A51_UPPER_SRCS += \
../src/SILABS_STARTUP.A51 

C_SRCS += \
../src/InitDevice.c \
../src/Interrupts.c \
../src/atecc508a.c \
../src/bsp.c \
../src/callback.c \
../src/descriptors.c \
../src/eeprom.c \
../src/i2c.c \
../src/idle.c \
../src/main.c \
../src/u2f-atecc.c \
../src/u2f.c \
../src/u2f_hid.c 

OBJS += \
./src/InitDevice.OBJ \
./src/Interrupts.OBJ \
./src/SILABS_STARTUP.OBJ \
./src/atecc508a.OBJ \
./src/bsp.OBJ \
./src/callback.OBJ \
./src/descriptors.OBJ \
./src/eeprom.OBJ \
./src/i2c.OBJ \
./src/idle.OBJ \
./src/main.OBJ \
./src/u2f-atecc.OBJ \
./src/u2f.OBJ \
./src/u2f_hid.OBJ 


# Each subdirectory must supply rules for building sources it contributes
src/%.OBJ: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	wine /home/pp/SimplicityStudio_v3/developer/toolchains/keil_8051/9.53/BIN/C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/InitDevice.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/firmware/inc/InitDevice.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/Interrupts.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/apps/u2f-zero/firmware/inc/i2c.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/%.OBJ: ../src/%.A51
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Assembler'
	wine /home/pp/SimplicityStudio_v3/developer/toolchains/keil_8051/9.53/BIN/A51 "@$(patsubst %.OBJ,%.__ia,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/atecc508a.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/apps/u2f-zero/firmware/inc/atecc508a.h /home/pp/apps/u2f-zero/firmware/inc/i2c.h /home/pp/apps/u2f-zero/firmware/inc/eeprom.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/bsp.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h

src/callback.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/idle.h /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/descriptors.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h

src/eeprom.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/eeprom.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/i2c.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/i2c.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/idle.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/idle.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

src/main.OBJ: /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/apps/u2f-zero/firmware/inc/atecc508a.h /home/pp/apps/u2f-zero/firmware/inc/InitDevice.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/apps/u2f-zero/firmware/inc/eeprom.h /home/pp/apps/u2f-zero/firmware/inc/idle.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/apps/u2f-zero/firmware/inc/i2c.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/apps/u2f-zero/firmware/inc/u2f.h /home/pp/apps/u2f-zero/firmware/tests/tests.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h

src/u2f-atecc.OBJ: /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/apps/u2f-zero/firmware/inc/u2f.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/apps/u2f-zero/firmware/inc/eeprom.h /home/pp/apps/u2f-zero/firmware/inc/atecc508a.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/u2f.OBJ: /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/apps/u2f-zero/firmware/inc/u2f.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h

src/u2f_hid.OBJ: /home/pp/apps/u2f-zero/firmware/inc/app.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h /home/pp/apps/u2f-zero/firmware/inc/bsp.h /home/pp/apps/u2f-zero/firmware/inc/u2f_hid.h /home/pp/apps/u2f-zero/firmware/inc/u2f.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Register_Enums.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Lib/efm8_usb/inc/efm8_usb.h /home/pp/apps/u2f-zero/firmware/inc/descriptors.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/inc/SI_EFM8UB1_Defs.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h /home/pp/apps/u2f-zero/firmware/inc/config/usbconfig.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/EFM8UB1/peripheral_driver/inc/usb_0.h /home/pp/SimplicityStudio_v3/developer/sdks/si8051/v3/Device/shared/si8051Base/endian.h /home/pp/apps/u2f-zero/firmware/inc/config/efm8_config.h


