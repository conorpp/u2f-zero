EESchema Schematic File Version 2
LIBS:u2f-zero-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:siliconlabs
LIBS:atmel_cryptoauth
LIBS:discrete
LIBS:debug
LIBS:u2f-zero-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L EFM8UB1 E1
U 1 1 56857441
P 5200 4500
F 0 "E1" H 5850 5100 60  0000 C CNN
F 1 "EFM8UB1" H 5900 5200 60  0000 C CNN
F 2 "footprints:EFM8UB1" H 4700 5000 60  0001 C CNN
F 3 "" H 4700 5000 60  0000 C CNN
	1    5200 4500
	1    0    0    -1  
$EndComp
$Comp
L ATECC508A-RESCUE-u2f-zero A1
U 1 1 56857313
P 4750 2350
F 0 "A1" H 5050 2650 60  0000 C CNN
F 1 "ATECC508A" H 4650 2650 60  0000 C CNN
F 2 "footprints:UDFN-8Pad" H 4350 2600 60  0001 C CNN
F 3 "" H 4350 2600 60  0000 C CNN
	1    4750 2350
	-1   0    0    1   
$EndComp
Text Label 4350 4450 2    60   ~ 0
GND
Text Label 6050 4700 0    60   ~ 0
GND
$Comp
L C C3
U 1 1 56857DEB
P 5350 5600
F 0 "C3" H 5375 5700 50  0000 L CNN
F 1 "0.1uF" H 5375 5500 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 5388 5450 50  0001 C CNN
F 3 "" H 5350 5600 50  0000 C CNN
	1    5350 5600
	0    -1   1    0   
$EndComp
$Comp
L C C4
U 1 1 56857E44
P 5350 5950
F 0 "C4" H 5375 6050 50  0000 L CNN
F 1 "4.7uF" H 5375 5850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5388 5800 50  0001 C CNN
F 3 "" H 5350 5950 50  0000 C CNN
	1    5350 5950
	0    1    1    0   
$EndComp
Text Label 4200 2900 0    60   ~ 0
+5V
$Comp
L DF5A5.6JE Z1
U 1 1 56857EAF
P 3450 4300
F 0 "Z1" H 3050 4350 60  0000 C CNN
F 1 "DF5A5.6JE" H 2900 4450 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-553" H 3450 4300 60  0001 C CNN
F 3 "" H 3450 4300 60  0000 C CNN
	1    3450 4300
	1    0    0    -1  
$EndComp
Text Label 5900 5600 0    60   ~ 0
GND
Text Label 4900 6150 0    60   ~ 0
+5V
Text Label 3650 3900 2    60   ~ 0
+5V
Text Label 3450 3900 2    60   ~ 0
GND
Text Label 3850 4750 0    60   ~ 0
HD-
Text Label 4050 4600 2    60   ~ 0
HD+
$Comp
L R R1
U 1 1 56857B9B
P 7750 4600
F 0 "R1" V 7830 4600 50  0000 C CNN
F 1 "100" V 7750 4600 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 7680 4600 50  0001 C CNN
F 3 "" H 7750 4600 50  0000 C CNN
	1    7750 4600
	-1   0    0    1   
$EndComp
NoConn ~ 5050 5300
Text Notes 3050 3750 0    60   ~ 0
Protect from ESD
Text Notes 3350 4900 0    60   ~ 0
Host USB data
Text Notes 7450 3600 0    60   ~ 0
Secure element for EC
$Comp
L CA_RGB-RESCUE-u2f-zero RGB1
U 1 1 5686DEFD
P 7000 4450
F 0 "RGB1" H 7250 4600 60  0000 C CNN
F 1 "CA_RGB" H 6900 4600 60  0000 C CNN
F 2 "footprints:LED-0606" H 7800 4500 60  0001 C CNN
F 3 "" H 7000 4500 60  0000 C CNN
	1    7000 4450
	1    0    0    1   
$EndComp
$Comp
L GND #PWR01
U 1 1 5686E5B0
P 6550 5850
F 0 "#PWR01" H 6550 5600 50  0001 C CNN
F 1 "GND" H 6550 5700 50  0000 C CNN
F 2 "" H 6550 5850 50  0000 C CNN
F 3 "" H 6550 5850 50  0000 C CNN
	1    6550 5850
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG02
U 1 1 5686E60B
P 6850 5850
F 0 "#FLG02" H 6850 5945 50  0001 C CNN
F 1 "PWR_FLAG" H 6850 6030 50  0000 C CNN
F 2 "" H 6850 5850 50  0000 C CNN
F 3 "" H 6850 5850 50  0000 C CNN
	1    6850 5850
	-1   0    0    1   
$EndComp
Text Label 6850 5650 0    60   ~ 0
+5V
Text Label 6550 5650 0    60   ~ 0
GND
$Comp
L SW_PUSH SW1
U 1 1 5685E9F9
P 4750 3250
F 0 "SW1" H 4900 3360 50  0000 C CNN
F 1 "SW_PUSH" H 4750 3170 50  0000 C CNN
F 2 "footprints:u2f-button" H 4750 3250 50  0001 C CNN
F 3 "" H 4750 3250 50  0000 C CNN
	1    4750 3250
	1    0    0    -1  
$EndComp
NoConn ~ 3250 4000
Text Label 5200 5300 3    60   ~ 0
C2CK
Text Label 5350 5300 3    60   ~ 0
C2D
Text Label 6100 3200 3    60   ~ 0
C2CK
Text Label 6200 3200 3    60   ~ 0
C2D
$Comp
L debug-pin TX1
U 1 1 56A3F7EB
P 5700 3350
F 0 "TX1" V 5500 2850 60  0000 C CNN
F 1 "debug-pin" H 5800 3100 60  0001 C CNN
F 2 "footprints:debug" H 5700 3350 60  0001 C CNN
F 3 "" H 5700 3350 60  0000 C CNN
	1    5700 3350
	-1   0    0    1   
$EndComp
$Comp
L debug-pin RX1
U 1 1 56A3F89E
P 5800 3350
F 0 "RX1" V 5600 2850 60  0000 C CNN
F 1 "debug-pin" H 5900 3100 60  0001 C CNN
F 2 "footprints:debug" H 5800 3350 60  0001 C CNN
F 3 "" H 5800 3350 60  0000 C CNN
	1    5800 3350
	-1   0    0    1   
$EndComp
$Comp
L debug-pin C2CK1
U 1 1 56A3F8D6
P 5900 3350
F 0 "C2CK1" V 5700 2800 60  0000 C CNN
F 1 "debug-pin" H 6000 3100 60  0001 C CNN
F 2 "footprints:debug" H 5900 3350 60  0001 C CNN
F 3 "" H 5900 3350 60  0000 C CNN
	1    5900 3350
	-1   0    0    1   
$EndComp
$Comp
L debug-pin C2D1
U 1 1 56A3F90D
P 6000 3350
F 0 "C2D1" V 5800 2800 60  0000 C CNN
F 1 "debug-pin" H 6100 3100 60  0001 C CNN
F 2 "footprints:debug" H 5950 3500 60  0001 C CNN
F 3 "" H 6000 3350 60  0000 C CNN
	1    6000 3350
	-1   0    0    1   
$EndComp
Text Label 5750 3300 0    60   ~ 0
TX
Text Label 5550 3400 0    60   ~ 0
RX
NoConn ~ 4750 5300
NoConn ~ 4150 2400
Text Label 5500 2200 0    60   ~ 0
GND
NoConn ~ 5350 2500
NoConn ~ 5350 2400
NoConn ~ 5350 2300
Text Label 4450 3250 2    60   ~ 0
GND
Wire Wire Line
	4900 5300 4900 6150
Wire Wire Line
	4900 5950 5200 5950
Wire Wire Line
	4900 5600 5200 5600
Connection ~ 4900 5950
Wire Wire Line
	5500 5950 5750 5950
Wire Wire Line
	5750 5950 5750 5600
Connection ~ 5750 5600
Connection ~ 4900 5600
Wire Wire Line
	5500 5600 5900 5600
Wire Wire Line
	6850 5850 6850 5650
Wire Wire Line
	6550 5650 6550 5850
Wire Wire Line
	3300 4600 3300 4750
Wire Wire Line
	3450 4000 3450 3900
Wire Wire Line
	3650 4000 3650 3900
Wire Wire Line
	5250 3700 5250 3300
Wire Wire Line
	5250 3300 5900 3300
Wire Wire Line
	5900 3300 5900 3200
Wire Wire Line
	5400 3700 5400 3400
Wire Wire Line
	5400 3400 6000 3400
Wire Wire Line
	6000 3400 6000 3200
Wire Wire Line
	4150 2500 4100 2500
Wire Wire Line
	4100 2500 4100 2900
Wire Wire Line
	4100 2900 4200 2900
Wire Wire Line
	4150 2300 4050 2300
Wire Wire Line
	4050 2300 4050 4150
Wire Wire Line
	4050 4150 4350 4150
Wire Wire Line
	4150 2200 4000 2200
Wire Wire Line
	4000 2200 4000 4300
Wire Wire Line
	4000 4300 4350 4300
Wire Wire Line
	5350 2200 5500 2200
Wire Wire Line
	5100 3250 5100 3700
Wire Wire Line
	5100 3250 5050 3250
Wire Wire Line
	4950 3700 4950 3500
Wire Wire Line
	4950 3500 5100 3500
Connection ~ 5100 3500
Wire Wire Line
	5550 3700 6350 3700
Wire Wire Line
	6350 3700 6350 4100
Wire Wire Line
	6350 4100 6550 4100
Wire Wire Line
	6050 4250 6550 4250
Wire Wire Line
	6550 4400 6050 4400
Wire Wire Line
	7500 4250 7750 4250
Wire Wire Line
	7750 4250 7750 4450
Wire Wire Line
	7750 4750 7750 4950
Text Label 7750 4950 0    60   ~ 0
+5V
Wire Wire Line
	3300 4750 4350 4750
Wire Wire Line
	3600 4600 4350 4600
NoConn ~ 6050 4550
NoConn ~ 6050 4850
Text Label 4850 3700 2    60   ~ 0
GND
$EndSCHEMATC
