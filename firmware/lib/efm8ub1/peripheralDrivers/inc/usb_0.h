/***************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 ******************************************************************************/

#ifndef __SILICON_LABS_EFM8_USB_0_H__
#define __SILICON_LABS_EFM8_USB_0_H__

#include "SI_EFM8UB1_Register_Enums.h"
#include <stdint.h>
#include <stdbool.h>
#include "efm8_config.h"

/******************************************************************************/

/**
 *
 * @addtogroup usb_0_group USB0 Driver
 * @{
 *
 * @brief Peripheral driver for USB 0
 *
 * # Introduction #
 *
 * This module provides an API for using the USB0 peripheral.
 * The API provides access to the USB hardware. A full-featured
 * USB stack (EFM8 USB Library) is available in the SDK at "\lib\efm8_usb."
 * The primary purpose of this USB peripheral driver is to abstract hardware
 * accesses so that the EFM8 USB Library can run on multiple EFM8 devices
 * (e.g. EFM8UB1, EFM8UB2). However, this driver can also be used to build
 * custom USB stacks and applications in cases where greater optimization or
 * performance than what the EFM8 USB Library provides is required.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup usb_0_runtime USB0 Runtime API
 * @brief
 * Functions and macros to access the USB hardware.
 * @{
 ******************************************************************************/

// -------------------------------
// Macros

/***************************************************************************//**
 * @brief       Reads an indirect USB register
 * @details     Sets USB0ADR and polls on the busy bit.
 *              When the macro completes, the value can be read from USB0DAT.
 * @param       addr
 *              The address of the USB indirect register to read
 * @return      The value of the USB indirect register is held in USB0DAT.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern uint8_t USB_READ_BYTE(uint8_t addr);
#else
#define USB_READ_BYTE(addr) \
  do \
  { \
    USB0ADR = (USB0ADR_BUSY__SET | (addr)); \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Writes an indirect USB register
 * @details     Sets USB0ADR, writes a value to USB0DAT, and waits for the busy
 *              bit to clear.
 * @param       addr
 *              The address of the USB indirect register to read
 * @param       dat
 *              The value to write to the USB indirect register
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_WRITE_BYTE(uint8_t addr, uint8_t dat);
#else
#define USB_WRITE_BYTE(addr, dat) \
  do \
  { \
    USB0ADR = (addr); \
    USB0DAT = (dat); \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Sets bits in an indirect USB register
 * @details     Sets the bits in the bitmask of the indirect USB register
 *              without disturbing the value of other bits in the indirect
 *              register.
 * @param       addr
 *              The address of the USB indirect register to write
 * @param       bitmask
 *              The bits to set
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SET_BITS(uint8_t addr, uint8_t bitmask);
#else
#define USB_SET_BITS(addr, bitmask) \
  do \
  { \
    USB0ADR = (USB0ADR_BUSY__SET | (addr)); \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    USB0DAT = (USB0DAT | (bitmask)); \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Clears bits in an indirect USB register
 * @details     Clears the bits in the bitmask of an indirect USB register
 *              without disturbing the value of other bits in the indirect
 *              register.
 * @param       addr
 *              The address of the USB indirect register to write
 * @param       bitmask
 *              The bits to clear
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_CLEAR_BITS(uint8_t addr, uint8_t bitmask);
#else
#define USB_CLEAR_BITS(addr, bitmask) \
  do \
  { \
    USB0ADR = (USB0ADR_BUSY__SET | (addr)); \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    USB0DAT = (USB0DAT & ~(bitmask)); \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Enables USB interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableInts(void);
#else
#define USB_EnableInts() \
  do \
  { \
    SFRPAGE = PG2_PAGE; \
    EIE2 |= EIE2_EUSB0__ENABLED; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Disables USB interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableInts(void);
#else
#define USB_DisableInts() \
  do \
  { \
    SFRPAGE = PG2_PAGE; \
    EIE2 &= ~EIE2_EUSB0__ENABLED; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB interrupt enabler
 * @return      TRUE if USB interrupts are enabled, FALSE otherwise.
 ******************************************************************************/
bool USB_GetIntsEnabled(void);

/***************************************************************************//**
 * @brief       Enables VBUS detection
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_VbusDetectEnable(void);
#else
#define USB_VbusDetectEnable() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF |= USB0CF_VBUSEN__ENABLED; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Disables VBUS detection
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_VbusDetectDisable(void);
#else
#define USB_VbusDetectDisable() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF &= ~USB0CF_VBUSEN__ENABLED; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Check status of VBUS signal
 * @return      TRUE if VBUS signal is present, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsVbusOn(void);
#else
#define USB_IsVbusOn() ((bool) P3_B1)
#endif

/***************************************************************************//**
 * @brief       Enables the USB pull-up resistor
 * @details     Enables either the D+ or the D- pull-up resistor, depending on
 *              whether @ref USB_SelectFullSpeed() or @ref USB_SelectLowSpeed()
 *              was previously called.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnablePullUpResistor(void);
#else
#define USB_EnablePullUpResistor() \
    do \
    { \
      SFRPAGE = PG3_PAGE; \
      USB0XCN |= USB0XCN_PREN__PULL_UP_ENABLED; \
    } while (0)
#endif

/***************************************************************************//**
 * @brief       Disables the USB pull-up resistor
 * @details     Disables either the D+ or the D- pull-up resistor, depending on
 *              whether @ref USB_SelectFullSpeed() or @ref USB_SelectLowSpeed()
 *              was previously called.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisablePullUpResistor(void);
#else
#define USB_DisablePullUpResistor() \
    do \
    { \
      SFRPAGE = PG3_PAGE; \
      USB0XCN &= ~USB0XCN_PREN__PULL_UP_ENABLED; \
    } while (0)
#endif

/***************************************************************************//**
 * @brief       Enables the USB transceiver
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableTransceiver(void);
#else
#define USB_EnableTransceiver() \
    do \
    { \
      SFRPAGE = PG3_PAGE; \
      USB0XCN |= USB0XCN_PHYEN__ENABLED; \
    } while (0)
#endif

/***************************************************************************//**
 * @brief       Disables the USB transceiver
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableTransceiver(void);
#else
#define USB_DisableTransceiver() \
    do \
    { \
      SFRPAGE = PG3_PAGE; \
      USB0XCN &= ~USB0XCN_PHYEN__ENABLED; \
    } while (0)
#endif

/***************************************************************************//**
 * @brief       Puts the USB in full-speed mode.
 * @details     Configures the USB to operate as a full-speed device by
 *              enabling the D+ pull-up resistor. After calling this
 *              function, the user must call @ref USB_EnablePullUpResistor().
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SelectFullSpeed(void);
#else
#define USB_SelectFullSpeed() \
    do \
    { \
      SFRPAGE = PG3_PAGE; \
      USB0XCN |= USB0XCN_SPEED__FULL_SPEED; \
    } while (0)
#endif

/***************************************************************************//**
 * @brief       Puts the USB in low-speed mode.
 * @details     Configures the USB to operate as a low-speed device by
 *              enabling the D- pull-up resistor. After calling this
 *              function, the user must call @ref USB_EnablePullUpResistor().
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SelectLowSpeed(void);
#else
#define USB_SelectLowSpeed() \
    do \
    { \
      SFRPAGE = PG3_PAGE; \
      USB0XCN &= ~USB0XCN_SPEED__FULL_SPEED; \
    } while (0)
#endif

/***************************************************************************//**
 * @brief       Suspends the transceiver
 * @details     Puts the USB transceiver in suspend mode.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SuspendTransceiver(void);
#else
#define USB_SuspendTransceiver() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0XCN &= ~(USB0XCN_PHYEN__ENABLED \
                 | USB0XCN_Dp__HIGH \
                 | USB0XCN_Dn__HIGH); \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the internal oscillator as the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetClockIntOsc(void);
#else
#define USB_SetClockIntOsc() \
    do \
    { \
      SFRPAGE = PG3_PAGE; \
      USB0CF &= ~USB0CF_USBCLK__FMASK; \
    } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the internal oscillator / 8 as the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetClockIntOscDiv8(void);
#else
#define USB_SetClockIntOscDiv8() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF &= ~USB0CF_USBCLK__FMASK; \
    USB0CF |= USB0CF_USBCLK__HFOSC1_DIV_8; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the external oscillator as the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetClockExtOsc(void);
#else
#define USB_SetClockExtOsc() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF &= ~USB0CF_USBCLK__FMASK; \
    USB0CF |= USB0CF_USBCLK__EXTOSC; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the external oscillator / 2 as the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetClockExtOscDiv2(void);
#else
#define USB_SetClockExtOscDiv2() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF &= ~USB0CF_USBCLK__FMASK; \
    USB0CF |= USB0CF_USBCLK__EXTOSC_DIV_2; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the external oscillator / 3 as the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetClockExtOscDiv3(void);
#else
#define USB_SetClockExtOscDiv3() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF &= ~USB0CF_USBCLK__FMASK; \
    USB0CF |= USB0CF_USBCLK__EXTOSC_DIV_3; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the external oscillator / 4 as the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetClockExtOscDiv4(void);
#else
#define USB_SetClockExtOscDiv4() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF &= ~USB0CF_USBCLK__FMASK; \
    USB0CF |= USB0CF_USBCLK__EXTOSC_DIV_4; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the low-frequency oscillator as the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetClockLfo(void);
#else
#define USB_SetClockLfo() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    USB0CF &= ~USB0CF_USBCLK__FMASK; \
    USB0CF |= USB0CF_USBCLK__LFOSC; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Selects the normal setting for the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetNormalClock(void);
#else
#define USB_SetNormalClock() USB_SetClockIntOsc()
#endif

/***************************************************************************//**
 * @brief       Selects the low-power setting for the USB clock
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetSuspendClock(void);
#else
#define USB_SetSuspendClock() USB_SetClockIntOscDiv8()
#endif

/***************************************************************************//**
 * @brief       Suspends REG1
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SuspendRegulator(void);
#else
#define USB_SuspendRegulator() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    REG1CN |= REG1CN_SUSEN__SUSPEND; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Does not use regulator low-power modes
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SuspendRegulatorFastWake(void);
#else
#define USB_SuspendRegulatorFastWake()
#endif

/***************************************************************************//**
 * @brief       Takes REG0 and REG1 out of suspend
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_UnsuspendRegulator(void);
#else
#define USB_UnsuspendRegulator() \
  do \
  { \
    SFRPAGE = PG3_PAGE; \
    REG1CN &= ~REG1CN_SUSEN__SUSPEND; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Determine if the internal regulator is enabled
 * @return      TRUE if the internal regulator is enabled, FALSE otherwise
 ******************************************************************************/
bool USB_IsRegulatorEnabled(void);

/***************************************************************************//**
 * @brief       Disable the prefetch engine
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
void USB_DisablePrefetch(void);
#else
#define USB_DisablePrefetch() \
  do \
  { \
    SFRPAGE = PG2_PAGE; \
    PFE0CN &= ~(PFE0CN_PFEN__ENABLED | PFE0CN_FLRT__SYSCLK_BELOW_50_MHZ); \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Enable the prefetch engine
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
void USB_EnablePrefetch(void);
#else
#define USB_EnablePrefetch() \
  do \
  { \
    SFRPAGE = PG2_PAGE; \
    PFE0CN |= (PFE0CN_PFEN__ENABLED | PFE0CN_FLRT__SYSCLK_BELOW_50_MHZ); \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Determine if the prefetch engine is enabled
 * @return      TRUE if prefetch engine is enabled, FALSE otherwise.
 ******************************************************************************/
bool USB_IsPrefetchEnabled(void);

/***************************************************************************//**
 * @brief       Suspends internal oscillator
 ******************************************************************************/
void USB_SuspendOscillator(void);

/***************************************************************************//**
 * @brief       Enables clock recovery in full speed mode
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableFullSpeedClockRecovery(void);
#else
#define USB_EnableFullSpeedClockRecovery() \
    USB_WRITE_BYTE(CLKREC, (CLKREC_CRE__ENABLED | 0x0F))
#endif

/***************************************************************************//**
 * @brief       Enables clock recovery in low speed mode
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableLowSpeedClockRecovery(void);
#else
#define USB_EnableLowSpeedClockRecovery() \
  USB_WRITE_BYTE(CLKREC, \
                  (CLKREC_CRE__ENABLED \
                   | CLKREC_CRLOW__LOW_SPEED \
                   | 0x0F))
#endif

/***************************************************************************//**
 * @brief       Disables clock recovery
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableClockRecovery(void);
#else
#define USB_DisableClockRecovery() USB_WRITE_BYTE(CLKREC, 0x0F)
#endif

/***************************************************************************//**
 * @brief       Sets the USB function address
 * @param       addr
 *   USB Function Address value
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetAddress(uint8_t addr);
#else
#define USB_SetAddress(addr) \
  USB_WRITE_BYTE(FADDR, (FADDR_UPDATE__SET | (addr)))
#endif

/***************************************************************************//**
 * @brief       Disable the USB Inhibit feature
 * @details     The USB block is inhibited after a power-on-reset or an
 *              asynchronous reset. Software should disable the inhibit bit
 *              after all USB and transceiver initialization is complete.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableInhibit(void);
#else
#define USB_DisableInhibit() \
  USB_WRITE_BYTE(POWER, (POWER_USBINH__ENABLED | POWER_SUSEN__ENABLED))
#endif

/***************************************************************************//**
 * @brief       Forces a USB reset
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_ForceReset(void);
#else
#define USB_ForceReset() USB_WRITE_BYTE(POWER, POWER_USBRST__SET)
#endif

/***************************************************************************//**
 * @brief       Forces USB resume signaling
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_ForceResume(void);
#else
#define USB_ForceResume() \
  USB_WRITE_BYTE(POWER, (POWER_RESUME__START | POWER_SUSEN__ENABLED))
#endif

/***************************************************************************//**
 * @brief       Clears USB resume signaling
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_ClearResume(void);
#else
#define USB_ClearResume() USB_WRITE_BYTE(POWER, POWER_SUSEN__ENABLED)
#endif

/***************************************************************************//**
 * @brief       Enables USB suspend detection
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableSuspendDetection(void);
#else
#define USB_EnableSuspendDetection() USB_WRITE_BYTE(POWER, POWER_SUSEN__ENABLED)
#endif

/***************************************************************************//**
 * @brief       Disables USB suspend detection
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableSuspendDetection(void);
#else
#define USB_DisableSuspendDetection() USB_WRITE_BYTE(POWER, 0)
#endif

/***************************************************************************//**
 * @brief       Setup End Serviced
 * @details     Software should call this function after servicing a Setup End
 *              event. Setup End is detected by calling usbGetSetupEnd
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_ServicedSetupEnd(void);
#else
#define USB_ServicedSetupEnd() \
  USB_WRITE_BYTE(E0CSR, E0CSR_SSUEND__SET)
#endif

/***************************************************************************//**
 * @brief       Out Packet Ready Serviced
 * @details     Software should call this function after servicing a received
 *              Endpoint 0 packet.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_Ep0ServicedOutPacketReady(void);
#else
#define USB_Ep0ServicedOutPacketReady() \
  USB_WRITE_BYTE(E0CSR, E0CSR_SOPRDY__SET)
#endif

/***************************************************************************//**
 * @brief       Sets In Packet Ready and Data End on Endpoint 0
 * @details     This should be called instead of @ref USB_Ep0SetInPacketReady()
 *              when sending the last packet of a setup data phase.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_Ep0SetLastInPacketReady(void);
#else
#define USB_Ep0SetLastInPacketReady() \
  USB_WRITE_BYTE(E0CSR, (E0CSR_INPRDY__SET | E0CSR_DATAEND__SET))
#endif

/***************************************************************************//**
 * @brief       Sets In Packet Ready and Data End on Endpoint 0
 * @details     This should be called instead of @ref USB_Ep0SetInPacketReady()
 *              when sending a zero-length packet.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_Ep0SetZLPInPacketReady(void);
#else
#define USB_Ep0SetZLPInPacketReady() \
  USB_WRITE_BYTE(E0CSR, (E0CSR_INPRDY__SET | E0CSR_DATAEND__SET))
#endif

/***************************************************************************//**
 * @brief       Serviced Out Packet Ready and Data End on Endpoint 0
 * @details     This should be called instead of @ref USB_ServicedSetupEnd()
 *              after servicing the last incoming data packet.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_Ep0SetLastOutPacketReady(void);
#else
#define USB_Ep0SetLastOutPacketReady() \
  USB_WRITE_BYTE(E0CSR, (E0CSR_SOPRDY__SET | E0CSR_DATAEND__SET))
#endif

/***************************************************************************//**
 * @brief       Sends a stall on Endpoint 0
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_Ep0SendStall(void);
#else
#define USB_Ep0SendStall() USB_WRITE_BYTE(E0CSR, E0CSR_SDSTL__SET)
#endif

/***************************************************************************//**
 * @brief       Clears sent stall condition on Endpoint 0
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_Ep0ClearSentStall(void);
#else
#define USB_Ep0ClearSentStall() USB_WRITE_BYTE(E0CSR, 0)
#endif

/***************************************************************************//**
 * @brief       Sets InPacketReady on Endpoint 0
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_Ep0SetInPacketReady(void);
#else
#define USB_Ep0SetInPacketReady() USB_WRITE_BYTE(E0CSR, E0CSR_INPRDY__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Start-of-Frame Interrupt
 * @param       CMINT_snapshot
 *              Snapshot of the CMINT register taken previously with the
 *              @ref USB_GetCommonInts() function.
 * @return      TRUE if Start-of-Frame Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsSofIntActive(uint8_t CMINT_snapshot);
#else
#define USB_IsSofIntActive(CMINT_snapshot) ((CMINT_snapshot) & CMINT_SOF__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Reset Interrupt
 * @param       CMINT_snapshot
 *              Snapshot of the CMINT register taken previously with the
 *              @ref USB_GetCommonInts() function.
 * @return      TRUE if USB Reset Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsResetIntActive(uint8_t CMINT_snapshot);
#else
#define USB_IsResetIntActive(CMINT_snapshot) \
  ((CMINT_snapshot) & CMINT_RSTINT__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Resume Interrupt
 * @param       CMINT_snapshot
 *              Snapshot of the CMINT register taken previously with the
 *              @ref USB_GetCommonInts() function.
 * @return      TRUE if USB Resume Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsResumeIntActive(uint8_t CMINT_snapshot);
#else
#define USB_IsResumeIntActive(CMINT_snapshot) \
  ((CMINT_snapshot) & CMINT_RSUINT__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Suspend Interrupt
 * @param       CMINT_snapshot
 *              Snapshot of the CMINT register taken previously with the
 *              @ref USB_GetCommonInts() function.
 * @return      TRUE if USB Suspend Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsSuspendIntActive(uint8_t CMINT_snapshot);
#else
#define USB_IsSuspendIntActive(CMINT_snapshot) \
  ((CMINT_snapshot) & CMINT_SUSINT__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Endpoint 0 Interrupt
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @return      TRUE if USB Endpoint 0 Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsEp0IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_IsEp0IntActive(IN1INT_snapshot) \
  ((IN1INT_snapshot) & IN1INT_EP0__SET)
#endif

/***************************************************************************//**
 * @brief       Returns 1 if any USB IN Interrupt is active
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @return      TRUE if any USB IN Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsInIntActive(uint8_t IN1INT_snapshot);
#else
#define USB_IsInIntActive(IN1INT_snapshot) \
  ((IN1INT_snapshot) & (IN1INT_IN1__SET | IN1INT_IN2__SET | IN1INT_IN3__SET))
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Endpoint 1 IN Interrupt
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @return      TRUE if USB Endpoint 1 IN Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsIn1IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_IsIn1IntActive(IN1INT_snapshot) \
  ((IN1INT_snapshot) & IN1INT_IN1__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Endpoint 2 IN Interrupt
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @return      TRUE if USB Endpoint 2 IN Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsIn2IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_IsIn2IntActive(IN1INT_snapshot) \
  ((IN1INT_snapshot) & IN1INT_IN2__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Endpoint 3 IN Interrupt
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @return      TRUE if USB Endpoint 3 IN Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsIn3IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_IsIn3IntActive(IN1INT_snapshot) \
  ((IN1INT_snapshot) & IN1INT_IN3__SET)
#endif

/***************************************************************************//**
 * @brief       Returns 1 if any USB Endpoint OUT Interrupt is active
 * @param       OUT1INT_snapshot
 *              Snapshot of the OUT1INT register taken previously with the
 *              @ref USB_GetOutInts() function.
 * @return      TRUE if any USB OUT Interrupt is active, FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsOutIntActive(uint8_t OUT1INT_snapshot);
#else
#define USB_IsOutIntActive(OUT1INT_snapshot) \
  ((OUT1INT_snapshot) \
     & (OUT1INT_OUT1__SET | OUT1INT_OUT2__SET | OUT1INT_OUT3__SET))
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Endpoint 1 OUT Interrupt
 * @param       OUT1INT_snapshot
 *              Snapshot of the OUT1INT register taken previously with the
 *              @ref USB_GetOutInts() function.
 * @return      TRUE if USB Endpoint 1 OUT Interrupt is active,
 *              FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsOut1IntActive(uint8_t OUT1INT_snapshot);
#else
#define USB_IsOut1IntActive(OUT1INT_snapshot) \
  ((OUT1INT_snapshot) & OUT1INT_OUT1__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Endpoint 2 OUT Interrupt
 * @param       OUT1INT_snapshot
 *              Snapshot of the OUT1INT register taken previously with the
 *              @ref USB_GetOutInts() function.
 * @return      TRUE if USB Endpoint 2 OUT Interrupt is active,
 *              FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsOut2IntActive(uint8_t OUT1INT_snapshot);
#else
#define USB_IsOut2IntActive(OUT1INT_snapshot) \
  ((OUT1INT_snapshot) & OUT1INT_OUT2__SET)
#endif

/***************************************************************************//**
 * @brief       Returns state of USB Endpoint 3 OUT Interrupt
 * @param       OUT1INT_snapshot
 *              Snapshot of the OUT1INT register taken previously with the
 *              @ref USB_GetOutInts() function.
 * @return      TRUE if USB Endpoint 3 OUT Interrupt is active,
 *              FALSE otherwise.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern bool USB_IsOut3IntActive(uint8_t OUT1INT_snapshot);
#else
#define USB_IsOut3IntActive(OUT1INT_snapshot) \
  ((OUT1INT_snapshot) & OUT1INT_OUT3__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the suspend interrupt flag to active
 * @param       CMINT_snapshot
 *              Snapshot of the CMINT register taken previously with the
 *              @ref USB_GetCommonInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetSuspendIntActive(uint8_t CMINT_snapshot);
#else
#define USB_SetSuspendIntActive(CMINT_snapshot) \
  (CMINT_snapshot |= CMINT_SUSINT__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the EP0 interrupt flag to active
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetEp0IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_SetEp0IntActive(IN1INT_snapshot) \
  (IN1INT_snapshot |= IN1INT_EP0__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the IN 1 interrupt flag to active
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetIn1IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_SetIn1IntActive(IN1INT_snapshot) \
  (IN1INT_snapshot |= IN1INT_IN1__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the IN 12interrupt flag to active
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetIn2IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_SetIn2IntActive(IN1INT_snapshot) \
  (IN1INT_snapshot |= IN1INT_IN2__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the IN 3 interrupt flag to active
 * @param       IN1INT_snapshot
 *              Snapshot of the IN1INT register taken previously with the
 *              @ref USB_GetInInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetIn3IntActive(uint8_t IN1INT_snapshot);
#else
#define USB_SetIn3IntActive(IN1INT_snapshot) \
  (IN1INT_snapshot |= IN1INT_IN3__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the OUT 1 interrupt flag to active
 * @param       OUT1INT_snapshot
 *              Snapshot of the OUT1INT register taken previously with the
 *              @ref USB_GetOutInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetOut1IntActive(uint8_t OUT1INT_snapshot);
#else
#define USB_SetOut1IntActive(OUT1INT_snapshot) \
  (OUT1INT_snapshot |= OUT1INT_OUT1__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the OUT 2 interrupt flag to active
 * @param       OUT1INT_snapshot
 *              Snapshot of the OUT1INT register taken previously with the
 *              @ref USB_GetOutInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetOut2IntActive(uint8_t OUT1INT_snapshot);
#else
#define USB_SetOut2IntActive(OUT1INT_snapshot) \
  (OUT1INT_snapshot |= OUT1INT_OUT2__SET)
#endif

/***************************************************************************//**
 * @brief       Sets the OUT 3 interrupt flag to active
 * @param       OUT1INT_snapshot
 *              Snapshot of the OUT1INT register taken previously with the
 *              @ref USB_GetOutInts() function.
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetOut3IntActive(uint8_t OUT1INT_snapshot);
#else
#define USB_SetOut3IntActive(OUT1INT_snapshot) \
  (OUT1INT_snapshot |= OUT1INT_OUT3__SET)
#endif

/***************************************************************************//**
 * @brief       Enables USB Start-of-Frame, Reset, Resume, and
 *              Suspend Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableDeviceInts(void);
#else
#define USB_EnableDeviceInts() \
  USB_WRITE_BYTE(CMIE, \
                  (CMIE_SOFE__ENABLED \
                   | CMIE_RSTINTE__ENABLED \
                   | CMIE_RSUINTE__ENABLED \
                   | CMIE_SUSINTE__ENABLED));
#endif

/***************************************************************************//**
 * @brief       Enables USB Start-of-Frame Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableSofInt(void);
#else
#define USB_EnableSofInt() USB_SET_BITS(CMIE, CMIE_SOFE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Start-of-Frame Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableSofInt(void);
#else
#define USB_DisableSofInt() USB_CLEAR_BITS(CMIE, CMIE_SOFE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Reset Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableResetInt(void);
#else
#define USB_EnableResetInt() USB_SET_BITS(CMIE, CMIE_RSTINTE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Reset Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableResetInt(void);
#else
#define USB_DisableResetInt() USB_CLEAR_BITS(CMIE, CMIE_RSTINTE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Resume Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableResumeInt(void);
#else
#define USB_EnableResumeInt() USB_SET_BITS(CMIE, CMIE_RSUINTE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Resume Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableResumeInt(void);
#else
#define USB_DisableResumeInt() USB_CLEAR_BITS(CMIE, CMIE_RSUINTE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Suspend Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableSuspendInt(void);
#else
#define USB_EnableSuspendInt() USB_SET_BITS(CMIE, CMIE_SUSINTE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Suspend Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableSuspendInt(void);
#else
#define USB_DisableSuspendInt() USB_CLEAR_BITS(CMIE, CMIE_SUSINTE__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 0 Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableEp0Int(void);
#else
#define USB_EnableEp0Int() USB_SET_BITS(IN1IE, IN1IE_EP0E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 0 Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableEp0Int(void);
#else
#define USB_DisableEp0Int() USB_CLEAR_BITS(IN1IE, IN1IE_EP0E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 1 IN Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableIn1Int(void);
#else
#define USB_EnableIn1Int() USB_SET_BITS(IN1IE, IN1IE_IN1E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 1 IN Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableIn1Int(void);
#else
#define USB_DisableIn1Int() USB_CLEAR_BITS(IN1IE, IN1IE_IN1E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 2 IN Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableIn2Int(void);
#else
#define USB_EnableIn2Int() USB_SET_BITS(IN1IE, IN1IE_IN2E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 2 IN Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableIn2Int(void);
#else
#define USB_DisableIn2Int() USB_CLEAR_BITS(IN1IE, IN1IE_IN2E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 3 IN Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableIn3Int(void);
#else
#define USB_EnableIn3Int() USB_SET_BITS(IN1IE, IN1IE_IN3E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 3 IN Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableIn3Int(void);
#else
#define USB_DisableIn3Int() USB_CLEAR_BITS(IN1IE, IN1IE_IN3E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 1 OUT Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableOut1Int(void);
#else
#define USB_EnableOut1Int() USB_SET_BITS(OUT1IE, OUT1IE_OUT1E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 1 OUT Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableOut1Int(void);
#else
#define USB_DisableOut1Int() USB_CLEAR_BITS(OUT1IE, OUT1IE_OUT1E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 2 OUT Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableOut2Int(void);
#else
#define USB_EnableOut2Int() USB_SET_BITS(OUT1IE, OUT1IE_OUT2E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 2 OUT Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableOut2Int(void);
#else
#define USB_DisableOut2Int() USB_CLEAR_BITS(OUT1IE, OUT1IE_OUT2E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 3 OUT Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableOut3Int(void);
#else
#define USB_EnableOut3Int() USB_SET_BITS(OUT1IE, OUT1IE_OUT3E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 3 OUT Interrupts
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableOut3Int(void);
#else
#define USB_DisableOut3Int() USB_CLEAR_BITS(OUT1IE, OUT1IE_OUT3E__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 1
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableEp1(void);
#else
#define USB_EnableEp1() USB_SET_BITS(EENABLE, EENABLE_EEN1__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 1
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableEp1(void);
#else
#define USB_DisableEp1() USB_CLEAR_BITS(EENABLE, EENABLE_EEN1__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 2
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableEp2(void);
#else
#define USB_EnableEp2() USB_SET_BITS(EENABLE, EENABLE_EEN2__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 2
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableEp2(void);
#else
#define USB_DisableEp2() USB_CLEAR_BITS(EENABLE, EENABLE_EEN2__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables USB Endpoint 3
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableEp3(void);
#else
#define USB_EnableEp3() USB_SET_BITS(EENABLE, EENABLE_EEN3__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables USB Endpoint 3
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableEp3(void);
#else
#define USB_DisableEp3() USB_CLEAR_BITS(EENABLE, EENABLE_EEN3__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Configures Endpoint N for OUT
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnDirectionOut(void);
#else
#define USB_EpnDirectionOut() USB_CLEAR_BITS(EINCSRH, EINCSRH_DIRSEL__IN);
#endif

/***************************************************************************//**
 * @brief       Configures Endpoint N for IN
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnDirectionIn(void);
#else
#define USB_EpnDirectionIn() USB_SET_BITS(EINCSRH, EINCSRH_DIRSEL__IN);
#endif

/***************************************************************************//**
 * @brief       Enables split mode on Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnEnableSplitMode(void);
#else
#define USB_EpnEnableSplitMode() \
    USB_SET_BITS(EINCSRH, EINCSRH_SPLIT__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables split mode Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnDisableSplitMode(void);
#else
#define USB_EpnDisableSplitMode() \
    USB_CLEAR_BITS(EINCSRH, EINCSRH_SPLIT__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Resets the IN endpoint data toggle to '0'
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInClearDataToggle(void);
#else
#define USB_EpnInClearDataToggle() USB_SET_BITS(EINCSRL, EINCSRL_CLRDT__BMASK);
#endif

/***************************************************************************//**
 * @brief       Clears sent stall condition on Endpoint N IN
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInClearSentStall(void);
#else
#define USB_EpnInClearSentStall() USB_WRITE_BYTE(EINCSRL, 0);
#endif

/***************************************************************************//**
 * @brief       Sends a stall for each IN token on Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInStall(void);
#else
#define USB_EpnInStall() USB_WRITE_BYTE(EINCSRL, EINCSRL_SDSTL__SET);
#endif

/***************************************************************************//**
 * @brief       Stops stalling for each IN token on Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInEndStall(void);
#else
#define USB_EpnInEndStall() USB_WRITE_BYTE(EINCSRL, 0);
#endif

/***************************************************************************//**
 * @brief       Stops stalling for each IN token on Endpoint N and clears
 *              the data toggle
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInEndStallAndClearDataToggle(void);
#else
#define USB_EpnInEndStallAndClearDataToggle() \
    USB_WRITE_BYTE(EINCSRL, EINCSRL_CLRDT__BMASK);
#endif

/***************************************************************************//**
 * @brief       Flushes In Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInFlush(void);
#else
#define USB_EpnInFlush() \
  do \
  { \
    USB_WRITE_BYTE(EINCSRL, EINCSRL_FLUSH__SET); \
    do \
    { \
      USB_READ_BYTE(EINCSRL); \
    } while (USB0DAT & EINCSRL_FLUSH__SET); \
  } while (0);
#endif

/***************************************************************************//**
 * @brief       Clears underrun condition on In Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInClearUnderrun(void);
#else
#define USB_EpnInClearUnderrun() USB_CLEAR_BITS(EINCSRL, EINCSRL_UNDRUN__SET);
#endif

/***************************************************************************//**
 * @brief       Sets InPacketReady on In Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnSetInPacketReady(void);
#else
#define USB_EpnSetInPacketReady() USB_SET_BITS(EINCSRL, EINCSRL_INPRDY__SET);
#endif

/***************************************************************************//**
 * @brief       Enables double buffering on In Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInEnableDoubleBuffer(void);
#else
#define USB_EpnInEnableDoubleBuffer() \
    USB_SET_BITS(EINCSRH, EINCSRH_DBIEN__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables double buffering on In Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInDisableDoubleBuffer(void);
#else
#define USB_EpnInDisableDoubleBuffer() \
    USB_CLEAR_BITS(EINCSRH, EINCSRH_DBIEN__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Configures In Endpoint N for Interrupt/Bulk Mode
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInEnableInterruptBulkMode(void);
#else
#define USB_EpnInEnableInterruptBulkMode() \
    USB_CLEAR_BITS(EINCSRH, EINCSRH_ISO__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Configures In Endpoint N for Isochronous Mode
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInEnableIsochronousMode(void);
#else
#define USB_EpnInEnableIsochronousMode() \
    USB_SET_BITS(EINCSRH, EINCSRH_ISO__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables forced data toggle on In Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInEnableForcedDataToggle(void);
#else
#define USB_EpnInEnableForcedDataToggle() \
    USB_SET_BITS(EINCSRH, EINCSRH_FCDT__ALWAYS_TOGGLE);
#endif

/***************************************************************************//**
 * @brief       Disables forced data toggle on In Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnInDisableForcedDataToggle(void);
#else
#define USB_EpnInDisableForcedDataToggle() \
    USB_CLEAR_BITS(EINCSRH, EINCSRH_FCDT__ALWAYS_TOGGLE);
#endif

/***************************************************************************//**
 * @brief       Resets the OUT endpoint data toggle to '0'
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutClearDataToggle(void);
#else
#define USB_EpnOutClearDataToggle() \
    USB_SET_BITS(EOUTCSRL, EOUTCSRL_CLRDT__BMASK);
#endif

/***************************************************************************//**
 * @brief       Clears sent stall condition on Endpoint N OUT
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutClearSentStall(void);
#else
#define USB_EpnOutClearSentStall() \
    USB_CLEAR_BITS(EOUTCSRL, EOUTCSRL_STSTL__BMASK);
#endif

/***************************************************************************//**
 * @brief       Sends a stall for each OUT token on Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutStall(void);
#else
#define USB_EpnOutStall() \
    USB_SET_BITS(EOUTCSRL, EOUTCSRL_SDSTL__SET);
#endif

/***************************************************************************//**
 * @brief       Stops stalling for each OUT token on Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutEndStall(void);
#else
#define USB_EpnOutEndStall() USB_CLEAR_BITS(EOUTCSRL, EOUTCSRL_SDSTL__SET);
#endif

/***************************************************************************//**
 * @brief       Stops stalling for each OUT token on Endpoint N and clears
 *              the data toggle
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutEndStallAndClearDataToggle(void);
#else
#define USB_EpnOutEndStallAndClearDataToggle() \
  do \
  { \
    USB_READ_BYTE(EOUTCSRL); \
    USB0DAT &= ~EOUTCSRL_SDSTL__SET; \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    USB0DAT |= EOUTCSRL_CLRDT__BMASK; \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
  } while (0);
#endif

/***************************************************************************//**
 * @brief       Flushes OUT Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutFlush(void);
#else
#define USB_EpnOutFlush() \
  do \
  { \
    USB_WRITE_BYTE(EOUTCSRL, EOUTCSRL_FLUSH__SET); \
    do \
    { \
      USB_READ_BYTE(EOUTCSRL); \
    } while (USB0DAT & EOUTCSRL_FLUSH__SET); \
  } while (0);
#endif

/***************************************************************************//**
 * @brief       Clears overrun condition on OUT Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutClearOverrun(void);
#else
#define USB_EpnOutClearOverrun() USB_CLEAR_BITS(EOUTCSRL, EOUTCSRL_OVRUN__SET);
#endif

/***************************************************************************//**
 * @brief       Clears OutPacketReady on  Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnClearOutPacketReady(void);
#else
#define USB_EpnClearOutPacketReady() \
    USB_CLEAR_BITS(EOUTCSRL, EOUTCSRL_OPRDY__SET);
#endif

/***************************************************************************//**
 * @brief       Enables double buffering on OUT Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutEnableDoubleBuffer(void);
#else
#define USB_EpnOutEnableDoubleBuffer() \
    USB_SET_BITS(EOUTCSRH, EOUTCSRH_DBIEN__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Disables double buffering on OUT Endpoint N
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutDisableDoubleBuffer(void);
#else
#define USB_EpnOutDisableDoubleBuffer() \
    USB_CLEAR_BITS(EOUTCSRH, EOUTCSRH_DBIEN__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Configures OUT Endpoint N for Interrupt/Bulk Mode
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutEnableInterruptBulkMode(void);
#else
#define USB_EpnOutEnableInterruptBulkMode() \
    USB_CLEAR_BITS(EOUTCSRH, EOUTCSRH_ISO__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Configures OUT Endpoint N for Isochronous Mode
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EpnOutEnableIsochronousMode(void);
#else
#define USB_EpnOutEnableIsochronousMode() \
    USB_SET_BITS(EOUTCSRH, EOUTCSRH_ISO__ENABLED);
#endif

/***************************************************************************//**
 * @brief       Enables FIFO read
 * @param       fifoNum
 *              FIFO to read
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableReadFIFO(uint8_t fifoNum);
#else
#define USB_EnableReadFIFO(fifoNum) \
  do \
  { \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    USB0ADR = (USB0ADR_BUSY__SET \
               | USB0ADR_AUTORD__ENABLED \
               | (FIFO0 | fifoNum)); \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Disables FIFO read
 * @param       fifoNum
 *              FIFO that was read from
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableReadFIFO(uint8_t fifoNum);
#else
#define USB_DisableReadFIFO(fifoNum)
#endif

/***************************************************************************//**
 * @brief       Reads a byte from the FIFO
 * @param       readDat
 *              Memory location to write the byte read from the FIFO
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_GetFIFOByte(uint8_t * readDat);
#else
#define USB_GetFIFOByte(readDat) \
  do \
  { \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    readDat = USB0DAT; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Reads the last byte from the FIFO
 * @details     The last read must be done with the AUTORD bit cleared.
 *              This prevents the read from triggering another read
 *              immediately thereafter.
 * @param       readDat
 *              Memory location to write the byte read from the FIFO
 * @param       fifoNum
 *              FIFO to read
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_GetLastFIFOByte(uint8_t * readDat, uint8_t fifoNum);
#else
#define USB_GetLastFIFOByte(readDat, fifoNum) \
  do \
  { \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    USB0ADR = (FIFO0 | fifoNum);\
    readDat = USB0DAT; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Enables FIFO write
 * @param       fifoNum
 *              FIFO to write
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_EnableWriteFIFO(uint8_t fifoNum);
#else
#define USB_EnableWriteFIFO(fifoNum) \
  do \
  { \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    USB0ADR = (FIFO0 | fifoNum); \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Disables FIFO write
 * @param       fifoNum
 *              FIFO that was written to
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_DisableWriteFIFO(uint8_t fifoNum);
#else
#define USB_DisableWriteFIFO(fifoNum)
#endif

/***************************************************************************//**
 * @brief       Writes a byte to the FIFO
 * @param       writeDat
 *              Data to write to the FIFO
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SetFIFOByte(uint8_t writeDat);
#else
#define USB_SetFIFOByte(writeDat) \
  do \
  { \
    while (USB0ADR & USB0ADR_BUSY__SET) {} \
    USB0DAT = writeDat; \
  } while (0)
#endif

/***************************************************************************//**
 * @brief       Saves the current SFR page
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_SaveSfrPage();
#else
#define USB_SaveSfrPage()  uint8_t SfrPageSave = SFRPAGE
#endif

/***************************************************************************//**
 * @brief       Restores the SFR page
 * @note        @ref USB_SaveSfrPage() must be called before calling this macro
 * @note        This function is implemented as a macro.
 ******************************************************************************/
#ifdef IS_DOXYGEN
extern void USB_RestoreSfrPage();
#else
#define USB_RestoreSfrPage() SFRPAGE = SfrPageSave
#endif

// -------------------------------
// Function Prototypes

/***************************************************************************//**
 * @brief       Writes a value to INDEX
 * @param       epsel
 *              Endpoint index to target
 ******************************************************************************/
void USB_SetIndex(uint8_t epsel);

/***************************************************************************//**
 * @brief       Reads the USB common interrupt register
 * @return      Value of CMINT
 ******************************************************************************/
uint8_t USB_GetCommonInts(void);

/***************************************************************************//**
 * @brief       Reads the USB in interrupt register
 * @return      Value of IN1INT
 ******************************************************************************/
uint8_t USB_GetInInts(void);

/***************************************************************************//**
 * @brief       Reads the out interrupt register
 * @return      Value of OUT1INT
 ******************************************************************************/
uint8_t USB_GetOutInts(void);

/***************************************************************************//**
 * @brief       Reads the value in INDEX
 * @return      Value of INDEX
 ******************************************************************************/
uint8_t USB_GetIndex(void);

/***************************************************************************//**
 * @brief       Determines if the USB is currently suspended
 * @return      TRUE if USB is in suspend mode
 ******************************************************************************/
bool USB_IsSuspended(void);

/***************************************************************************//**
 * @brief       Gets Setup End state
 * @return      TRUE when a control transaction end before software has
 *              set the DATAEND bit.
 ******************************************************************************/
bool USB_GetSetupEnd(void);

/***************************************************************************//**
 * @brief       Determines if STALL was send on Endpoint 0
 * @return      TRUE after a STALL was sent on Endpoint 0
 ******************************************************************************/
bool USB_Ep0SentStall(void);

/***************************************************************************//**
 * @brief       Determines if Out Packet Ready is set on Endpoint 0
 * @return      TRUE if Out Packet Ready is set on Endpoint 0
 ******************************************************************************/
bool USB_Ep0InPacketReady(void);

/***************************************************************************//**
 * @brief       Determines if In Packet Ready is set on Endpoint 0
 * @return      TRUE if In Packet Ready is set on Endpoint 0
 ******************************************************************************/
bool USB_Ep0OutPacketReady(void);

/***************************************************************************//**
 * @brief       Gets Endpoint 0 data count
 * @return      Number of received data bytes in the Endpoint 0 FIFO
 ******************************************************************************/
uint8_t USB_Ep0GetCount(void);

/***************************************************************************//**
 * @brief       Checks if stall was sent on IN Endpoint N
 * @return      TRUE if stall was sent on IN Endpoint N, FALSE otherwise
 ******************************************************************************/
bool USB_EpnInGetSentStall(void);

/***************************************************************************//**
 * @brief       Checks if stall was sent on OUT Endpoint N
 * @return      TRUE if stall was sent on OUT Endpoint N, FALSE otherwise
 ******************************************************************************/
bool USB_EpnGetInPacketReady(void);

/***************************************************************************//**
 * @brief       Checks if stall was sent on OUT Endpoint N
 * @return      TRUE if stall was sent on OUT Endpoint N, FALSE otherwise
 ******************************************************************************/
bool USB_EpnOutGetSentStall(void);

/***************************************************************************//**
 * @brief       Gets OutPacketReady on OUT Endpoint N
 * @return      TRUE if OUTPacketReady is set, FALSE otherwise
 ******************************************************************************/
bool USB_EpnGetOutPacketReady(void);

/***************************************************************************//**
 * @brief       Gets number of bytes in the OUT FIFO
 *              OUT packet
 * @return      Number of bytes in the FIFO from the last received
 *              packet
 ******************************************************************************/
uint16_t USB_EpOutGetCount(void);

/***************************************************************************//**
 * @brief       Reads the USB frame number
 * @return      The frame number on the most recent SOF packet
 ******************************************************************************/
uint16_t USB_GetSofNumber(void);

/***************************************************************************//**
 * @brief       Aborts pending IN transactions on the selected endpoint
 * @param       fifoNum
 *              Endpoint to abort
 ******************************************************************************/
void USB_AbortInEp(uint8_t fifoNum);

/***************************************************************************//**
 * @brief       Aborts pending OUT transactions on the selected endpoint
 * @param       fifoNum
 *              Endpoint to abort
 ******************************************************************************/
void USB_AbortOutEp(uint8_t fifoNum);

/***************************************************************************//**
 * @brief       Activates the selected endpoint
 * @param       ep
 *              Endpoint to access
 * @param       packetSize
 *              Maximum packet size for endpoint
 * @param       inDir
 *              Set to 1 if endpoint is IN, 0 if it is OUT
 * @param       splitMode
 *              Set to 1 if endpoint is in split mode, 0 if it is not
 * @param       isoMode
 *              Set to 1 if endpoint is in isochronous mode, 0 if it is not
 ******************************************************************************/
void USB_ActivateEp(uint8_t ep,
                    uint16_t packetSize,
                    bool inDir,
                    bool splitMode,
                    bool isoMode);

/**  @} (end addtogroup usb_0_runtime USB0 Runtime API) */
/**  @} (end addtogroup usb_0_group USB0 Driver) */

// -----------------------------------------------------------------------------
// Error Checking

// -------------------------------
// Verify that the maximum packet size specified for an endpoint is not too
// large for that endpoint

#ifdef SLAB_USB_EP1IN_USED
  #if SLAB_USB_EP1IN_USED
    #if SLAB_USB_EP1IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #error Isochronous transfers are not supported on Endpoint 1.
    #else           // #if SLAB_USB_EP1IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP1IN_MAX_PACKET_SIZE > 64
        #error EP1IN packet size too large. Interrupt/Bulk packet size must be 64 bytes or less.
      #endif        // #if SLAB_USB_EP1IN_MAX_PACKET_SIZE > 64
    #endif          // #if SLAB_USB_EP1IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
  #endif            // #if SLAB_USB_EP1IN_USED
#endif              // #ifdef SLAB_USB_EP1IN_USED

#ifdef SLAB_USB_EP1OUT_USED
  #if SLAB_USB_EP1OUT_USED
    #if SLAB_USB_EP1OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #error Isochronous transfers are not supported on Endpoint 1.
    #else           // #if SLAB_USB_EP1OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP1OUT_MAX_PACKET_SIZE > 64
        #error EP1OUT packet size too large. Interrupt/Bulk packet size must be 64 bytes or less.
      #endif        // #if SLAB_USB_EP1OUT_MAX_PACKET_SIZE > 64
    #endif          // #if SLAB_USB_EP1OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
  #endif            // #if SLAB_USB_EP1OUT_USED
#endif              // #ifdef SLAB_USB_EP1OUT_USED

#ifdef SLAB_USB_EP2IN_USED
  #if SLAB_USB_EP2IN_USED
    #if SLAB_USB_EP2IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #error Isochronous transfers are not supported on Endpoint 2.
    #else           // #if SLAB_USB_EP2IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP2IN_MAX_PACKET_SIZE > 64
        #error EP2IN packet size too large. Interrupt/Bulk packet size must be 64 bytes or less.
      #endif        // #if SLAB_USB_EP2IN_MAX_PACKET_SIZE > 64
    #endif          // #if SLAB_USB_EP2IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
  #endif            // #if SLAB_USB_EP2IN_USED
#endif              // #ifdef SLAB_USB_EP2IN_USED

#ifdef SLAB_USB_EP2OUT_USED
  #if SLAB_USB_EP2OUT_USED
    #if SLAB_USB_EP2OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #error Isochronous transfers are not supported on Endpoint 2.
    #else           // #if SLAB_USB_EP2OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP2OUT_MAX_PACKET_SIZE > 64
        #error EP2OUT packet size too large. Interrupt/Bulk packet size must be 64 bytes or less.
      #endif        // #if SLAB_USB_EP2OUT_MAX_PACKET_SIZE > 64
    #endif          // #if SLAB_USB_EP2OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
  #endif            // #if SLAB_USB_EP2OUT_USED
#endif              // #ifdef SLAB_USB_EP2OUT_USED

#ifdef SLAB_USB_EP3IN_USED
  #if SLAB_USB_EP3IN_USED
    #if SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP3OUT_USED
        #if SLAB_USB_EP3IN_MAX_PACKET_SIZE > 256
          #error EP3IN packet size too large. FIFO 3 split mode packet size must be 256 bytes or less.
        #endif      // #if SLAB_USB_EP3IN_MAX_PACKET_SIZE > 256
      #else         // #if SLAB_USB_EP3OUT_USED
        #if SLAB_USB_EP3IN_MAX_PACKET_SIZE > 512
          #error EP3IN packet size too large. FIFO 3 packet size must be 512 bytes or less.
        #endif      // #if SLAB_USB_EP3IN_MAX_PACKET_SIZE > 512
      #endif        // #if SLAB_USB_EP3OUT_USED
    #else           // #if SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP3IN_MAX_PACKET_SIZE > 64
        #error EP3IN packet size too large. Interrupt/Bulk packet size must be 64 bytes or less.
      #endif        // #if SLAB_USB_EP3IN_MAX_PACKET_SIZE > 64
    #endif          // #if SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
  #endif            // #if SLAB_USB_EP3IN_USED
#endif              // #ifdef SLAB_USB_EP3IN_USED

#ifdef SLAB_USB_EP3OUT_USED
  #if SLAB_USB_EP3OUT_USED
    #if SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP3IN_USED
        #if SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 256
          #error EP3OUT packet size too large. FIFO 3 split mode packet size must be 256 bytes or less.
        #endif      // #if SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 256
      #else         // #if SLAB_USB_EP3IN_USED
        #if SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 512
          #error EP3OUT packet size too large. FIFO 3 packet size must be 512 bytes or less.
        #endif      // #if SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 512
      #endif        // #if SLAB_USB_EP3IN_USED
    #else           // #if SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
      #if SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 64
        #error EP3OUT packet size too large. Interrupt/Bulk packet size must be 64 bytes or less.
      #endif        // #if SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 64
    #endif          // #if SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC
  #endif            // #if SLAB_USB_EP3OUT_USED
#endif              // #ifdef SLAB_USB_EP3OUT_USED

#endif  // __SILICON_LABS_EFM8_USB_0_H__
