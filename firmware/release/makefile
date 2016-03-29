################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Keil tools exit code 1 is warnings only, so don't treat as an error in make
RC := test $$? -lt 2

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include tests/subdir.mk
-include src/subdir.mk
-include lib/efm8ub1/peripheralDrivers/src/subdir.mk
-include lib/efm8_usb/src/subdir.mk
-include lib/efm8_assert/subdir.mk
-include subdir.mk
-include objects.mk

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: u2f-firmware.omf

# Tool invocations
u2f-firmware.omf: echo_path $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Keil 8051 Linker'
	wine /home/pp/SimplicityStudio_v3/developer/toolchains/keil_8051/9.53/BIN/LX51 "@u2f-firmware.lnp" || $(RC)
	@echo 'Finished building target: $@'

# Change from all upper case to the expected case
	@-mv "U2F-FIRMWARE.OMF.CRBUILD" "$@"

# Generate hex file: u2f-firmware.hex
	@wine /home/pp/SimplicityStudio_v3/developer/toolchains/keil_8051/9.53/BIN/Ohx51 "u2f-firmware.omf" "HEXFILE (u2f-firmware.hex)" "H386" 2>&1 >/dev/null

# Other Targets
clean:
	-$(RM) $(OBJS)$(EXECUTABLES) u2f-firmware.omf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
# echo the path
echo_path:
	@echo PATH=$$PATH
	@echo ' '

