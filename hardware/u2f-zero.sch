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
L ATECC508A-RESCUE-u2f-zero A1
U 1 1 56857313
P 5100 3050
F 0 "A1" H 5400 3350 60  0000 C CNN
F 1 "ATECC508A" H 5000 3350 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-8_3.9x4.9mm_Pitch1.27mm" H 4700 3300 60  0001 C CNN
F 3 "http://www.atmel.com/Images/Atmel-8923S-CryptoAuth-ATECC508A-Datasheet-Summary.pdf" H 4700 3300 60  0001 C CNN
F 4 "Atmel" H 5100 3050 60  0001 C CNN "MFG Name"
F 5 "ATECC508A-SSHDA-B" H 5100 3050 60  0001 C CNN "MFG Part Num"
F 6 "ATECC508A-SSHDA-B-ND" H 5100 3050 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/atmel/ATECC508A-SSHDA-B/ATECC508A-SSHDA-B-ND/5213053" H 5100 3050 60  0001 C CNN "Distributer Link"
	1    5100 3050
	-1   0    0    1   
$EndComp
$Comp
L C C3
U 1 1 56857DEB
P 5300 5750
F 0 "C3" H 5325 5850 50  0000 L CNN
F 1 "0.1uF" H 5325 5650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0402" H 5338 5600 50  0001 C CNN
F 3 "http://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 5300 5750 50  0001 C CNN
F 4 "Samsung Electro-Mechanics America, Inc" H 5300 5750 60  0001 C CNN "MFG Name"
F 5 "CL05A104MP5NNNC" H 5300 5750 60  0001 C CNN "MFG Part Num"
F 6 "1276-1443-1-ND" H 5300 5750 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/CL05A104MP5NNNC/1276-1443-1-ND/3889529" H 5300 5750 60  0001 C CNN "Distributer Link"
	1    5300 5750
	0    -1   1    0   
$EndComp
$Comp
L C C4
U 1 1 56857E44
P 5300 6100
F 0 "C4" H 5325 6200 50  0000 L CNN
F 1 "4.7uF" H 5325 6000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5338 5950 50  0001 C CNN
F 3 "http://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL_Series_MLCC_ds.pdf" H 5300 6100 50  0001 C CNN
F 4 "Samsung Electro-Mechanics America, Inc" H 5300 6100 60  0001 C CNN "MFG Name"
F 5 "CL10B475KQ8NQNC" H 5300 6100 60  0001 C CNN "MFG Part Num"
F 6 "1276-2087-1-ND" H 5300 6100 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/CL10B475KQ8NQNC/1276-2087-1-ND/3890173" H 5300 6100 60  0001 C CNN "Distributer Link"
	1    5300 6100
	0    1    1    0   
$EndComp
Text Label 4600 3600 0    60   ~ 0
+5V
$Comp
L DF5A5.6JE Z1
U 1 1 56857EAF
P 3250 3950
F 0 "Z1" H 2850 4000 60  0000 C CNN
F 1 "DF5A5.6JE" H 2700 4100 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-553" H 3250 3950 60  0001 C CNN
F 3 "http://optoelectronics.liteon.com/upload/download/DS22-2008-0044/S_110_LTST-C19HE1WT.pdf" H 3250 3950 60  0001 C CNN
F 4 "Toshiba Semiconductor and Storage" H 3250 3950 60  0001 C CNN "MFG Name"
F 5 "DF5A5.6JE,LM" H 3250 3950 60  0001 C CNN "MFG Part Num"
F 6 "DF5A5.6JELMCT-ND" H 3250 3950 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/DF5A5.6JE,LM/DF5A5.6JELMCT-ND/5403466" H 3250 3950 60  0001 C CNN "Distributer Link"
	1    3250 3950
	1    0    0    -1  
$EndComp
Text Label 5850 5750 0    60   ~ 0
GND
Text Label 4850 6300 0    60   ~ 0
+5V
Text Label 3450 3550 2    60   ~ 0
+5V
Text Label 3250 3550 2    60   ~ 0
GND
Text Label 3750 4500 2    60   ~ 0
HD-
Text Label 4150 4400 2    60   ~ 0
HD+
$Comp
L R R1
U 1 1 56857B9B
P 8100 4900
F 0 "R1" V 8180 4900 50  0000 C CNN
F 1 "100" V 8100 4900 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 8030 4900 50  0001 C CNN
F 3 "http://www.vishay.com/docs/20035/dcrcwe3.pdf" H 8100 4900 50  0001 C CNN
F 4 "Vishay Dale" H 8100 4900 60  0001 C CNN "MFG Name"
F 5 "CRCW0603100RFKEA" H 8100 4900 60  0001 C CNN "MFG Part Num"
F 6 "541-100HCT-ND" H 8100 4900 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/CRCW0603100RFKEA/541-100HCT-ND/1179695" H 8100 4900 60  0001 C CNN "Distributer Link"
	1    8100 4900
	-1   0    0    1   
$EndComp
Text Notes 2850 3400 0    60   ~ 0
Protect from ESD
Text Notes 2350 4450 0    60   ~ 0
Host USB data
Text Notes 4700 2700 0    60   ~ 0
Secure element for EC
$Comp
L CA_RGB-RESCUE-u2f-zero RGB1
U 1 1 5686DEFD
P 7050 4700
F 0 "RGB1" H 7300 4850 60  0000 C CNN
F 1 "CA_RGB" H 6950 4850 60  0000 C CNN
F 2 "footprints:LED-0606" H 7850 4750 60  0001 C CNN
F 3 "http://optoelectronics.liteon.com/upload/download/DS22-2008-0044/S_110_LTST-C19HE1WT.pdf" H 7050 4750 60  0001 C CNN
F 4 "Lite-On Inc" H 7050 4700 60  0001 C CNN "MFG Name"
F 5 "LTST-C19HE1WT" H 7050 4700 60  0001 C CNN "MFG Part Num"
F 6 "160-2162-1-ND" H 7050 4700 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/LTST-C19HE1WT/160-2162-1-ND/4866310" H 7050 4700 60  0001 C CNN "Distributer Link"
	1    7050 4700
	1    0    0    1   
$EndComp
$Comp
L GND #PWR01
U 1 1 5686E5B0
P 6700 5800
F 0 "#PWR01" H 6700 5550 50  0001 C CNN
F 1 "GND" H 6700 5650 50  0000 C CNN
F 2 "" H 6700 5800 50  0000 C CNN
F 3 "" H 6700 5800 50  0000 C CNN
	1    6700 5800
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG02
U 1 1 5686E60B
P 7100 5800
F 0 "#FLG02" H 7100 5895 50  0001 C CNN
F 1 "PWR_FLAG" H 7100 5980 50  0000 C CNN
F 2 "" H 7100 5800 50  0000 C CNN
F 3 "" H 7100 5800 50  0000 C CNN
	1    7100 5800
	-1   0    0    1   
$EndComp
Text Label 7100 5600 0    60   ~ 0
+5V
Text Label 6700 5600 0    60   ~ 0
GND
$Comp
L SW_PUSH SW1
U 1 1 5685E9F9
P 6500 5200
F 0 "SW1" H 6650 5310 50  0000 C CNN
F 1 "SW_PUSH" H 6500 5120 50  0000 C CNN
F 2 "footprints:u2f-button" H 6500 5200 50  0001 C CNN
F 3 "http://www.digikey.com/product-detail/en/e-switch/TL3305AF260QG/EG5353CT-ND/5816198" H 6500 5200 50  0001 C CNN
F 4 "E-Switch" H 6500 5200 60  0001 C CNN "MFG Name"
F 5 "TL3305AF260QG" H 6500 5200 60  0001 C CNN "MFG Part Num"
F 6 "EG5353CT-ND" H 6500 5200 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/e-switch/TL3305AF260QG/EG5353CT-ND/5816198" H 6500 5200 60  0001 C CNN "Distributer Link"
	1    6500 5200
	1    0    0    -1  
$EndComp
NoConn ~ 3050 3650
Text Label 6650 3400 3    60   ~ 0
C2CK
Text Label 6750 3400 3    60   ~ 0
C2D
$Comp
L debug-pin TX1
U 1 1 56A3F7EB
P 6250 3550
F 0 "TX1" V 6050 3050 60  0000 C CNN
F 1 "debug-pin" H 6350 3300 60  0001 C CNN
F 2 "footprints:debug" H 6250 3550 60  0001 C CNN
F 3 "" H 6250 3550 60  0000 C CNN
	1    6250 3550
	-1   0    0    1   
$EndComp
$Comp
L debug-pin RX1
U 1 1 56A3F89E
P 6350 3550
F 0 "RX1" V 6150 3050 60  0000 C CNN
F 1 "debug-pin" H 6450 3300 60  0001 C CNN
F 2 "footprints:debug" H 6350 3550 60  0001 C CNN
F 3 "" H 6350 3550 60  0000 C CNN
	1    6350 3550
	-1   0    0    1   
$EndComp
$Comp
L debug-pin C2CK1
U 1 1 56A3F8D6
P 6450 3550
F 0 "C2CK1" V 6250 3000 60  0000 C CNN
F 1 "debug-pin" H 6550 3300 60  0001 C CNN
F 2 "footprints:debug" H 6450 3550 60  0001 C CNN
F 3 "" H 6450 3550 60  0000 C CNN
	1    6450 3550
	-1   0    0    1   
$EndComp
$Comp
L debug-pin C2D1
U 1 1 56A3F90D
P 6550 3550
F 0 "C2D1" V 6350 3000 60  0000 C CNN
F 1 "debug-pin" H 6650 3300 60  0001 C CNN
F 2 "footprints:debug" H 6500 3700 60  0001 C CNN
F 3 "" H 6550 3550 60  0000 C CNN
	1    6550 3550
	-1   0    0    1   
$EndComp
Text Label 6100 4100 0    60   ~ 0
TX
Text Label 6550 4050 0    60   ~ 0
RX
NoConn ~ 4500 3100
Text Label 6050 2900 0    60   ~ 0
GND
NoConn ~ 5700 3200
NoConn ~ 5700 3100
NoConn ~ 5700 3000
Text Label 6800 5200 0    60   ~ 0
GND
Wire Wire Line
	4850 5750 4850 6300
Wire Wire Line
	4850 6100 5150 6100
Wire Wire Line
	4850 5750 5150 5750
Connection ~ 4850 6100
Wire Wire Line
	5450 6100 5700 6100
Wire Wire Line
	5700 6100 5700 5750
Connection ~ 5700 5750
Connection ~ 4850 5750
Wire Wire Line
	5450 5750 5850 5750
Wire Wire Line
	7100 5800 7100 5600
Wire Wire Line
	6700 5600 6700 5800
Wire Wire Line
	3100 4250 3100 4500
Wire Wire Line
	3250 3650 3250 3550
Wire Wire Line
	3450 3650 3450 3550
Wire Wire Line
	6450 3400 6450 4100
Wire Wire Line
	8100 5050 8100 5250
Text Label 8100 5250 0    60   ~ 0
+5V
Text Label 4550 5000 2    60   ~ 0
C2CK
Text Label 4550 5100 2    60   ~ 0
C2D
Wire Wire Line
	4550 5100 4800 5100
Wire Wire Line
	4800 5000 4550 5000
Wire Wire Line
	4800 4400 3800 4400
Wire Wire Line
	3800 4400 3800 4250
Wire Wire Line
	3800 4250 3400 4250
Wire Wire Line
	3100 4500 4800 4500
Wire Wire Line
	6600 4350 6250 4350
Wire Wire Line
	6250 4350 6250 4400
Wire Wire Line
	6250 4400 5950 4400
Wire Wire Line
	6600 4500 5950 4500
Wire Wire Line
	6600 4650 6250 4650
Wire Wire Line
	6250 4650 6250 4600
Wire Wire Line
	6250 4600 5950 4600
Wire Wire Line
	8100 4750 8100 4500
Wire Wire Line
	8100 4500 7550 4500
Wire Wire Line
	6450 4100 5950 4100
Wire Wire Line
	6550 3400 6550 4200
Wire Wire Line
	6550 4200 5950 4200
Wire Wire Line
	6200 5200 6200 5050
Wire Wire Line
	6200 5050 6100 5050
Wire Wire Line
	6100 5000 6100 5100
Wire Wire Line
	6100 5000 5950 5000
Wire Wire Line
	6100 5100 5950 5100
Connection ~ 6100 5050
Wire Wire Line
	4500 2900 4250 2900
Wire Wire Line
	4250 2900 4250 4200
Wire Wire Line
	4250 4200 4800 4200
Wire Wire Line
	4500 3000 4400 3000
Wire Wire Line
	4400 3000 4400 4100
Wire Wire Line
	4400 4100 4800 4100
Wire Wire Line
	4500 3200 4500 3600
Wire Wire Line
	4500 3600 4600 3600
Wire Wire Line
	6050 2900 5700 2900
Wire Wire Line
	4800 4300 4150 4300
Wire Wire Line
	4150 4300 4150 3800
Wire Wire Line
	4150 3800 3900 3800
Wire Wire Line
	3900 3800 3900 3700
Text Label 3900 3700 0    60   ~ 0
GND
Wire Wire Line
	4200 4800 4800 4800
Text Label 4200 4800 2    60   ~ 0
+5V
NoConn ~ 5950 4900
NoConn ~ 5950 4800
NoConn ~ 5950 4700
NoConn ~ 5950 4300
NoConn ~ 5950 4000
NoConn ~ 4800 4000
NoConn ~ 4800 4900
$Comp
L EFM8UB1_24pin E0
U 1 1 5705BC18
P 5350 4250
F 0 "E0" H 5800 4750 60  0000 C CNN
F 1 "EFM8UB1_24pin" H 5300 4750 60  0000 C CNN
F 2 "footprints:QSOP-24_3.9x8.7mm_Pitch0.635mm_fat" H 5650 4650 60  0001 C CNN
F 3 "https://www.silabs.com/Support%20Documents/TechnicalDocs/EFM8UB1_DataSheet.pdf" H 5350 4250 60  0001 C CNN
F 4 "Silicon Labs" H 5350 4250 60  0001 C CNN "MFG Name"
F 5 "EFM8UB11F16G-C-QSOP24" H 5350 4250 60  0001 C CNN "MFG Part Num"
F 6 "r	336-3411-5-ND" H 5350 4250 60  0001 C CNN "Distributer PN"
F 7 "http://www.digikey.com/product-detail/en/silicon-labs/EFM8UB11F16G-C-QSOP24/336-3411-5-ND/5592439" H 5350 4250 60  0001 C CNN "Distributer Link"
	1    5350 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 4700 4700 4700
Wire Wire Line
	4700 4600 4800 4600
$Comp
L debug-pin GND1
U 1 1 570602D3
P 6850 3550
F 0 "GND1" V 6650 3000 60  0000 C CNN
F 1 "debug-pin" H 6950 3300 60  0001 C CNN
F 2 "footprints:debug" H 6550 3650 60  0001 C CNN
F 3 "" H 6850 3550 60  0000 C CNN
	1    6850 3550
	-1   0    0    1   
$EndComp
Text Label 7050 3400 3    60   ~ 0
GND
Wire Wire Line
	4700 4700 4700 4600
$Comp
L u2f-fiducial F1
U 1 1 571C39BB
P 2600 5450
F 0 "F1" H 2450 5650 60  0000 C CNN
F 1 "u2f-fiducial" H 2950 5650 60  0000 C CNN
F 2 "footprints:u2f-fiducial" H 2600 5450 60  0001 C CNN
F 3 "" H 2600 5450 60  0000 C CNN
	1    2600 5450
	1    0    0    -1  
$EndComp
$Comp
L u2f-fiducial F2
U 1 1 571C3A1E
P 3100 5600
F 0 "F2" H 2950 5800 60  0000 C CNN
F 1 "u2f-fiducial" H 3450 5800 60  0000 C CNN
F 2 "footprints:u2f-fiducial" H 3100 5600 60  0001 C CNN
F 3 "" H 3100 5600 60  0000 C CNN
	1    3100 5600
	1    0    0    -1  
$EndComp
$Comp
L u2f-fiducial F3
U 1 1 571C3A85
P 3500 5750
F 0 "F3" H 3350 5950 60  0000 C CNN
F 1 "u2f-fiducial" H 3850 5950 60  0000 C CNN
F 2 "footprints:u2f-fiducial" H 3500 5750 60  0001 C CNN
F 3 "" H 3500 5750 60  0000 C CNN
	1    3500 5750
	1    0    0    -1  
$EndComp
$EndSCHEMATC
