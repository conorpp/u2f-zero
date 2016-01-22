/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "si_toolchain.h"
#include "efm8_usb.h"
#include <stdint.h>
#include <endian.h>

extern SI_SEGMENT_VARIABLE(myUsbDevice, USBD_Device_TypeDef, MEM_MODEL_SEG);

// -----------------------------------------------------------------------------
// Function Prototypes

// -------------------------------
// Memory-specific FIFO access functions
#ifdef SI_GPTR

static void USB_ReadFIFO_Idata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_IDATA), uint8_t fifoNum);
static void USB_WriteFIFO_Idata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_IDATA));

static void USB_ReadFIFO_Xdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_XDATA), uint8_t fifoNum);
static void USB_WriteFIFO_Xdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_XDATA));

#if SI_GPTR_MTYPE_PDATA != SI_GPTR_MTYPE_XDATA
static void USB_ReadFIFO_Pdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_PDATA), uint8_t fifoNum);
static void USB_WriteFIFO_Pdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_PDATA));
#endif

#if SI_GPTR_MTYPE_DATA != SI_GPTR_MTYPE_IDATA
static void USB_ReadFIFO_Data(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_DATA), uint8_t fifoNum);
static void USB_WriteFIFO_Data(uint8_t numBytes, uint8_t SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_DATA));
#endif

static void USB_WriteFIFO_Code(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_CODE));

#else

// -------------------------------
// Generic FIFO access functions
static void USB_ReadFIFO_Generic(uint8_t numBytes, uint8_t *dat, uint8_t fifoNum);
static void USB_WriteFIFO_Generic(uint8_t numBytes, uint8_t *dat);

#endif  // #ifdef SI_GPTR

// -----------------------------------------------------------------------------
// Functions

/***************************************************************************//**
 * @brief       Reads Isochronous data from the Endpoint FIFO
 * @param       fifoNum
 *              USB Endpoint FIFO to read
 * @param       numBytes
 *              Number of bytes to read from the FIFO
 * @param       dat
 *              Pointer to buffer to hold data read from the FIFO
 ******************************************************************************/
#if (SLAB_USB_EP3OUT_USED && (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC) && (SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 255))
// ----------------------------------------------------------------------------
// If Isochronous mode is enabled and the max packet size is greater than 255,
// break the FIFO reads up into multiple reads of 255 or less bytes.
// ----------------------------------------------------------------------------
void USB_ReadFIFOIso(uint8_t fifoNum, uint16_t numBytes, uint8_t *dat)
{
  uint8_t numBytesRead;

  // USB_ReadFIFO() accepts a maximum of 255 bytes. If the number of bytes to
  // send is greated than 255, call USB_ReadFIFO() multiple times.
  while (numBytes > 0)
  {
    numBytesRead = (numBytes > 255) ? 255 : numBytes;
    USB_ReadFIFO(fifoNum, numBytesRead, dat);
    numBytes -= numBytesRead;
    dat += numBytesRead;
  }
}
#else
#define USB_ReadFIFOIso(a, b, c) USB_ReadFIFO(a, b, c)
#endif

/***************************************************************************//**
 * @brief       Writes Isochronous data to the Endpoint FIFO
 * @param       fifoNum
 *              USB Endpoint FIFO to write
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to buffer hoding data to write to the FIFO
 ******************************************************************************/
#if (SLAB_USB_EP3IN_USED && (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC) && (SLAB_USB_EP3IN_MAX_PACKET_SIZE > 255))
// ----------------------------------------------------------------------------
// If Isochronous mode is enabled and the max packet size is greater than 255,
// break the FIFO writes up into multiple writes of 255 or less bytes.
// ----------------------------------------------------------------------------
void USB_WriteFIFOIso(uint8_t fifoNum, uint16_t numBytes, uint8_t *dat)
{
  uint8_t numBytesWrite;

  // USB_WriteFIFO() accepts a maximum of 255 bytes. If the number of bytes to
  // send is greated than 255, call USB_WriteFIFO() multiple times.
  while (numBytes > 0)
  {
    numBytesWrite = (numBytes > 255) ? 255 : numBytes;
    numBytes -= numBytesWrite;
    USB_WriteFIFO(fifoNum, numBytesWrite, dat, (numBytes == 0));
    dat += numBytesWrite;
  }
}
#else
#define USB_WriteFIFOIso(a, b, c) USB_WriteFIFO(a, b, c, true)
#endif

#if SLAB_USB_EP1IN_USED
/***************************************************************************//**
 * @brief       Handle Endpoint 1 IN transfer interrupt
 * @note        This function takes no parameters, but it uses the EP1IN status
 *              variables stored in @ref myUsbDevice.ep1in.
 ******************************************************************************/
void handleUsbIn1Int(void)
{
  uint8_t xferred;
  bool callback;

  USB_SetIndex(1);

  if (USB_EpnInGetSentStall())
  {
    USB_EpnInClearSentStall();
  }
  else if (myUsbDevice.ep1in.state == D_EP_TRANSMITTING)
  {
    xferred = (myUsbDevice.ep1in.remaining > SLAB_USB_EP1IN_MAX_PACKET_SIZE)
              ? SLAB_USB_EP1IN_MAX_PACKET_SIZE : myUsbDevice.ep1in.remaining;
    myUsbDevice.ep1in.remaining -= xferred;
    myUsbDevice.ep1in.buf += xferred;

    callback = myUsbDevice.ep1in.misc.bits.callback;

    // Load more data
    if (myUsbDevice.ep1in.remaining > 0)
    {
      USB_WriteFIFO(1,
                    (myUsbDevice.ep1in.remaining > SLAB_USB_EP1IN_MAX_PACKET_SIZE)
                      ? SLAB_USB_EP1IN_MAX_PACKET_SIZE
                      : myUsbDevice.ep1in.remaining,
                    myUsbDevice.ep1in.buf,
                    true);
    }
    else
    {
      myUsbDevice.ep1in.misc.bits.callback = false;
      myUsbDevice.ep1in.state = D_EP_IDLE;
    }

    if (callback == true)
    {
      USBD_XferCompleteCb(EP1IN, USB_STATUS_OK, xferred, myUsbDevice.ep1in.remaining);
    }

  }
}
#endif // SLAB_USB_EP1IN_USED

#if SLAB_USB_EP2IN_USED
/***************************************************************************//**
 * @brief       Handle Endpoint 2 IN transfer interrupt
 * @note        This function takes no parameters, but it uses the EP2IN status
 *              variables stored in @ref myUsbDevice.ep2in.
 ******************************************************************************/
void handleUsbIn2Int(void)
{
  uint8_t xferred;
  bool callback;

  USB_SetIndex(2);

  if (USB_EpnInGetSentStall())
  {
    USB_EpnInClearSentStall();
  }
  else if (myUsbDevice.ep2in.state == D_EP_TRANSMITTING)
  {
    xferred = (myUsbDevice.ep2in.remaining > SLAB_USB_EP2IN_MAX_PACKET_SIZE)
              ? SLAB_USB_EP2IN_MAX_PACKET_SIZE : myUsbDevice.ep2in.remaining;
    myUsbDevice.ep2in.remaining -= xferred;
    myUsbDevice.ep2in.buf += xferred;

    callback = myUsbDevice.ep2in.misc.bits.callback;

    // Load more data
    if (myUsbDevice.ep2in.remaining > 0)
    {
      USB_WriteFIFO(2,
                    (myUsbDevice.ep2in.remaining > SLAB_USB_EP2IN_MAX_PACKET_SIZE)
                      ? SLAB_USB_EP2IN_MAX_PACKET_SIZE
                      : myUsbDevice.ep2in.remaining,
                    myUsbDevice.ep2in.buf,
                    true);
    }
    else
    {
      myUsbDevice.ep2in.misc.bits.callback = false;
      myUsbDevice.ep2in.state = D_EP_IDLE;
    }

    if (callback == true)
    {
      USBD_XferCompleteCb(EP2IN, USB_STATUS_OK, xferred, myUsbDevice.ep2in.remaining);
    }

  }
}
#endif // SLAB_USB_EP2IN_USED

#if SLAB_USB_EP3IN_USED
/***************************************************************************//**
 * @brief       Handle Endpoint 3 IN transfer interrupt
 * @details     Endpoint 3 IN is the only IN endpoint that supports isochronous
 *              transfers.
 * @note        This function takes no parameters, but it uses the EP3IN status
 *              variables stored in @ref myUsbDevice.ep3in.
 ******************************************************************************/
void handleUsbIn3Int(void)
{
#if SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC
  uint16_t xferred, nextIdx;
#else
  uint8_t xferred;
  bool callback;
#endif

  USB_SetIndex(3);

  if (USB_EpnInGetSentStall())
  {
    USB_EpnInClearSentStall();
  }
  else if (myUsbDevice.ep3in.state == D_EP_TRANSMITTING)
  {
#if  ((SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_BULK) || (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_INTR))
    xferred = (myUsbDevice.ep3in.remaining > SLAB_USB_EP3IN_MAX_PACKET_SIZE)
              ? SLAB_USB_EP3IN_MAX_PACKET_SIZE : myUsbDevice.ep3in.remaining;
    myUsbDevice.ep3in.remaining -= xferred;
    myUsbDevice.ep3in.buf += xferred;
#endif

#if  ((SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_BULK) || (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_INTR))

    callback = myUsbDevice.ep3in.misc.bits.callback;

#elif (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC)
    if (myUsbDevice.ep3in.misc.bits.callback == true)
    {
      // In Isochronous mode, the meaning of the USBD_XferCompleteCb parameters changes:
      //   xferred is ignored
      //   remaining is the current index into the circular buffer
      //   the return value is the number of bytes to transmit in the next packet
      xferred = USBD_XferCompleteCb(EP3IN, USB_STATUS_OK, 0, myUsbDevice.ep3inIsoIdx);
      if (xferred == 0)
      {
        myUsbDevice.ep3in.misc.bits.inPacketPending = true;
        return;
      }
    }
#endif
    // Load more data
#if  ((SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_BULK) || (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_INTR))
    if (myUsbDevice.ep3in.remaining > 0)
    {
      USB_WriteFIFO(3,
                    (myUsbDevice.ep3in.remaining > SLAB_USB_EP3IN_MAX_PACKET_SIZE)
                      ? SLAB_USB_EP3IN_MAX_PACKET_SIZE
                      : myUsbDevice.ep3in.remaining,
                    myUsbDevice.ep3in.buf,
                    true);
    }
    else
    {
      myUsbDevice.ep3in.misc.bits.callback = false;
      myUsbDevice.ep3in.state = D_EP_IDLE;
    }

    if (callback == true)
    {
      USBD_XferCompleteCb(EP3IN, USB_STATUS_OK, xferred, myUsbDevice.ep3in.remaining);
    }
#elif (SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_ISOC)
    nextIdx = xferred + myUsbDevice.ep3inIsoIdx;
    myUsbDevice.ep3in.misc.bits.inPacketPending = false;

    // Check if the next index is past the end of the circular buffer.
    // If so, break the write up into two calls to USB_WriteFIFOIso()
    if (nextIdx > myUsbDevice.ep3in.remaining)
    {
      USB_WriteFIFOIso(3, myUsbDevice.ep3in.remaining - myUsbDevice.ep3inIsoIdx, &myUsbDevice.ep3in.buf[myUsbDevice.ep3inIsoIdx]);
      myUsbDevice.ep3inIsoIdx = nextIdx - myUsbDevice.ep3in.remaining;
      USB_WriteFIFOIso(3, myUsbDevice.ep3inIsoIdx, myUsbDevice.ep3in.buf);
    }
    else
    {
      USB_WriteFIFOIso(3, xferred, &myUsbDevice.ep3in.buf[myUsbDevice.ep3inIsoIdx]);
      myUsbDevice.ep3inIsoIdx = nextIdx;
    }
#endif // ( ( SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_BULK ) || ( SLAB_USB_EP3IN_TRANSFER_TYPE == USB_EPTYPE_INTR ) )
  }
}
#endif // SLAB_USB_EP3IN_USED

#if SLAB_USB_EP1OUT_USED
/***************************************************************************//**
 * @brief       Handle Endpoint 1 OUT transfer interrupt
 * @note        This function takes no parameters, but it uses the EP1OUT status
 *              variables stored in @ref myUsbDevice.ep1out.
 ******************************************************************************/
void handleUsbOut1Int(void)
{
  uint8_t count;

  USB_Status_TypeDef status;
  bool xferComplete = false;

  USB_SetIndex(1);

  if (USB_EpnOutGetSentStall())
  {
    USB_EpnOutClearSentStall();
  }
  else if (USB_EpnGetOutPacketReady())
  {
    count = USB_EpOutGetCount();

    // If USBD_Read() has not been called, return an error
    if (myUsbDevice.ep1out.state != D_EP_RECEIVING)
    {
      myUsbDevice.ep1out.misc.bits.outPacketPending = true;
      status = USB_STATUS_EP_ERROR;
    }
    // Check for overrun of user buffer
    else if (myUsbDevice.ep1out.remaining < count)
    {
      myUsbDevice.ep1out.state = D_EP_IDLE;
      myUsbDevice.ep1out.misc.bits.outPacketPending = true;
      status = USB_STATUS_EP_RX_BUFFER_OVERRUN;
    }
    else
    {
      USB_ReadFIFO(1, count, myUsbDevice.ep1out.buf);

      myUsbDevice.ep1out.misc.bits.outPacketPending = false;
      myUsbDevice.ep1out.remaining -= count;
      myUsbDevice.ep1out.buf += count;

      if ((myUsbDevice.ep1out.remaining == 0) || (count != SLAB_USB_EP1OUT_MAX_PACKET_SIZE))
      {
        myUsbDevice.ep1out.state = D_EP_IDLE;
        xferComplete = true;
      }

      status = USB_STATUS_OK;
      USB_EpnClearOutPacketReady();
    }
    if (myUsbDevice.ep1out.misc.bits.callback == true)
    {
      if (xferComplete == true)
      {
        myUsbDevice.ep1out.misc.bits.callback = false;
      }

      USBD_XferCompleteCb(EP1OUT, status, count, myUsbDevice.ep1out.remaining);
    }
  }
}
#endif  // EP1OUT_USED

#if SLAB_USB_EP2OUT_USED
/***************************************************************************//**
 * @brief       Handle Endpoint 2 OUT transfer interrupt
 * @note        This function takes no parameters, but it uses the EP2OUT status
 *              variables stored in @ref myUsbDevice.ep2out.
 ******************************************************************************/
void handleUsbOut2Int(void)
{
  uint8_t count;

  USB_Status_TypeDef status;
  bool xferComplete = false;

  USB_SetIndex(2);

  if (USB_EpnOutGetSentStall())
  {
    USB_EpnOutClearSentStall();
  }
  else if (USB_EpnGetOutPacketReady())
  {
    count = USB_EpOutGetCount();

    // If USBD_Read() has not been called, return an error
    if (myUsbDevice.ep2out.state != D_EP_RECEIVING)
    {
      myUsbDevice.ep2out.misc.bits.outPacketPending = true;
      status = USB_STATUS_EP_ERROR;
    }
    // Check for overrun of user buffer
    else if (myUsbDevice.ep2out.remaining < count)
    {
      myUsbDevice.ep2out.state = D_EP_IDLE;
      myUsbDevice.ep2out.misc.bits.outPacketPending = true;
      status = USB_STATUS_EP_RX_BUFFER_OVERRUN;
    }
    else
    {
      USB_ReadFIFO(2, count, myUsbDevice.ep2out.buf);

      myUsbDevice.ep2out.misc.bits.outPacketPending = false;
      myUsbDevice.ep2out.remaining -= count;
      myUsbDevice.ep2out.buf += count;

      if ((myUsbDevice.ep2out.remaining == 0) || (count != SLAB_USB_EP2OUT_MAX_PACKET_SIZE))
      {
        myUsbDevice.ep2out.state = D_EP_IDLE;
        xferComplete = true;
      }

      status = USB_STATUS_OK;
      USB_EpnClearOutPacketReady();
    }
    if (myUsbDevice.ep2out.misc.bits.callback == true)
    {
      if (xferComplete == true)
      {
        myUsbDevice.ep2out.misc.bits.callback = false;
      }

      USBD_XferCompleteCb(EP2OUT, status, count, myUsbDevice.ep2out.remaining);
    }
  }
}
#endif  // EP2OUT_USED

#if SLAB_USB_EP3OUT_USED
/***************************************************************************//**
 * @brief       Handle Endpoint 3 OUT transfer interrupt
 * @details     Endpoint 3 OUT is the only OUT endpoint that supports
 *              isochronous transfers.
 * @note        This function takes no parameters, but it uses the EP3OUT status
 *              variables stored in @ref myUsbDevice.ep3out.
 ******************************************************************************/
void handleUsbOut3Int(void)
{
#if (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC)
  uint16_t nextIdx;
#if (SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 255)
  uint16_t count;
#else
  uint8_t count;
#endif // ( SLAB_USB_EP3OUT_MAX_PACKET_SIZE > 255 )
#else
  uint8_t count;
#endif // ( SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC )

  USB_Status_TypeDef status;
  bool xferComplete = false;

  USB_SetIndex(3);

  if (USB_EpnOutGetSentStall())
  {
    USB_EpnOutClearSentStall();
  }
  else if (USB_EpnGetOutPacketReady())
  {
    count = USB_EpOutGetCount();

    // If USBD_Read() has not been called, return an error
    if (myUsbDevice.ep3out.state != D_EP_RECEIVING)
    {
      myUsbDevice.ep3out.misc.bits.outPacketPending = true;
      status = USB_STATUS_EP_ERROR;
    }
#if  ((SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_BULK) || (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_INTR))
    // Check for overrun of user buffer
    else if (myUsbDevice.ep3out.remaining < count)
    {
      myUsbDevice.ep3out.state = D_EP_IDLE;
      myUsbDevice.ep3out.misc.bits.outPacketPending = true;
      status = USB_STATUS_EP_RX_BUFFER_OVERRUN;
    }
#endif
    else
    {
#if  ((SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_BULK) || (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_INTR))
      USB_ReadFIFO(3, count, myUsbDevice.ep3out.buf);

      myUsbDevice.ep3out.remaining -= count;
      myUsbDevice.ep3out.buf += count;

      if ((myUsbDevice.ep3out.remaining == 0) || (count != SLAB_USB_EP3OUT_MAX_PACKET_SIZE))
      {
        myUsbDevice.ep3out.state = D_EP_IDLE;
        xferComplete = true;
      }
#elif (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC)
      nextIdx = count + myUsbDevice.ep3outIsoIdx;

      // In isochronous mode, a circular buffer is used to hold the data
      // If the next index into the circular buffer passes the end of the
      // buffer, make two calls to USB_ReadFIFOIso()
      if (nextIdx > myUsbDevice.ep3out.remaining)
      {
        USB_ReadFIFOIso(3, myUsbDevice.ep3out.remaining - myUsbDevice.ep3outIsoIdx, &myUsbDevice.ep3out.buf[myUsbDevice.ep3outIsoIdx]);
        myUsbDevice.ep3outIsoIdx = nextIdx - myUsbDevice.ep3out.remaining;
        USB_ReadFIFOIso(3, myUsbDevice.ep3outIsoIdx, myUsbDevice.ep3out.buf);
      }
      else
      {
        USB_ReadFIFOIso(3, count, &myUsbDevice.ep3out.buf[myUsbDevice.ep3outIsoIdx]);
        myUsbDevice.ep3outIsoIdx = nextIdx;
      }
#endif // ( ( SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_BULK ) || ( SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_INTR ) )

      myUsbDevice.ep3out.misc.bits.outPacketPending = false;
      status = USB_STATUS_OK;
      USB_EpnClearOutPacketReady();
    }
    if (myUsbDevice.ep3out.misc.bits.callback == true)
    {
      if (xferComplete == true)
      {
        myUsbDevice.ep3out.misc.bits.callback = false;
      }

#if  ((SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_BULK) || (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_INTR))
      USBD_XferCompleteCb(EP3OUT, status, count, myUsbDevice.ep3out.remaining);
#elif (SLAB_USB_EP3OUT_TRANSFER_TYPE == USB_EPTYPE_ISOC)

      // In Isochronous mode, the meaning of the USBD_XferCompleteCb parameters changes:
      //   xferred is the number of bytes received in the last packet
      //   remaining is the current index into the circular buffer
      USBD_XferCompleteCb(EP3OUT, status, count, myUsbDevice.ep3outIsoIdx);
#endif
    }
  }
}
#endif  // EP3OUT_USED

/***************************************************************************//**
 * @brief       Reads data from the USB FIFO
 * @param       fifoNum
 *              USB Endpoint FIFO to read
 * @param       numBytes
 *              Number of bytes to read from the FIFO
 * @param       dat
 *              Pointer to buffer to hold data read from the FIFO
 ******************************************************************************/
void USB_ReadFIFO(uint8_t fifoNum, uint8_t numBytes, uint8_t *dat)
{
  if (numBytes > 0)
  {
    USB_EnableReadFIFO(fifoNum);

    // Convert generic pointer to memory-specific pointer and call the
    // the corresponding memory-specific function, if possible.
    // The memory-specific functions are much faster than the generic functions.
#ifdef SI_GPTR

    switch (((SI_GEN_PTR_t *)&dat)->gptr.memtype)
    {
      case SI_GPTR_MTYPE_IDATA:
        USB_ReadFIFO_Idata(numBytes, dat, fifoNum);
        break;

      // For some compilers, IDATA and DATA are treated the same.
      // Only call the USB_ReadFIFO_Data() if the compiler differentiates
      // between DATA and IDATA.
#if (SI_GPTR_MTYPE_DATA != SI_GPTR_MTYPE_IDATA)
      case SI_GPTR_MTYPE_DATA:
        USB_ReadFIFO_Data(numBytes, dat, fifoNum);
        break;
#endif

      case SI_GPTR_MTYPE_XDATA:
        USB_ReadFIFO_Xdata(numBytes, dat, fifoNum);
        break;

      // For some compilers, XDATA and PDATA are treated the same.
      // Only call the USB_ReadFIFO_Pdata() if the compiler differentiates
      // between XDATA and PDATA.
#if (SI_GPTR_MTYPE_PDATA != SI_GPTR_MTYPE_XDATA)
      case SI_GPTR_MTYPE_PDATA:
        USB_ReadFIFO_Pdata(numBytes, dat, fifoNum);
        break;
#endif

      default:
        break;
    }

#else
    USB_ReadFIFO_Generic(numBytes, dat, fifoNum);
#endif  // #ifdef SI_GPTR

    USB_DisableReadFIFO(fifoNum);
  }
}

/***************************************************************************//**
 * @brief       Writes data to the USB FIFO
 * @param       fifoNum
 *              USB Endpoint FIFO to write
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to buffer hoding data to write to the FIFO
 * @param       txPacket
 *              If TRUE, the packet will be sent immediately after loading the
 *              FIFO
 *              If FALSE, the packet will be stored in the FIFO and the
 *              transmission must be started at a later time
 ******************************************************************************/
void USB_WriteFIFO(uint8_t fifoNum, uint8_t numBytes, uint8_t *dat, bool txPacket)
{
  USB_EnableWriteFIFO(fifoNum);

  // Convert generic pointer to memory-specific pointer and call the
  // the corresponding memory-specific function, if possible.
  // The memory-specific functions are much faster than the generic functions.
#ifdef SI_GPTR

  switch (((SI_GEN_PTR_t *)&dat)->gptr.memtype)
  {
    case SI_GPTR_MTYPE_IDATA:
      USB_WriteFIFO_Idata(numBytes, dat);
      break;

    // For some compilers, IDATA and DATA are treated the same.
    // Only call the USB_WriteFIFO_Data() if the compiler differentiates between
    // DATA and IDATA.
#if (SI_GPTR_MTYPE_DATA != SI_GPTR_MTYPE_IDATA)
    case SI_GPTR_MTYPE_DATA:
      USB_WriteFIFO_Data(numBytes, dat);
      break;
#endif

    case SI_GPTR_MTYPE_XDATA:
      USB_WriteFIFO_Xdata(numBytes, dat);
      break;

    // For some compilers, XDATA and PDATA are treated the same.
    // Only call the USB_WriteFIFO_Pdata() if the compiler differentiates
    // between XDATA and PDATA.
#if (SI_GPTR_MTYPE_PDATA != SI_GPTR_MTYPE_XDATA)
    case SI_GPTR_MTYPE_PDATA:
      USB_WriteFIFO_Pdata(numBytes, dat);
      break;
#endif

    case SI_GPTR_MTYPE_CODE:
      USB_WriteFIFO_Code(numBytes, dat);
      break;

    default:
      break;
  }

#else
  USB_WriteFIFO_Generic(numBytes, dat);
#endif  // #ifdef SI_GPTR

  USB_DisableWriteFIFO(fifoNum);

  if ((txPacket == true) && (fifoNum > 0))
  {
    USB_SetIndex(fifoNum);
    USB_EpnSetInPacketReady();
  }
}

// -----------------------------------------------------------------------------
// Memory-Specific FIFO Access Functions
//
// Memory-specific functions are much faster (more than 2x) than generic
// generic functions, so we will use memory-specific functions if possible.
// -----------------------------------------------------------------------------

#ifdef SI_GPTR
/***************************************************************************//**
 * @brief       Reads data from the USB FIFO to a buffer in IRAM
 * @param       numBytes
 *              Number of bytes to read from the FIFO
 * @param       dat
 *              Pointer to IDATA buffer to hold data read from the FIFO
 * @param       fifoNum
 *              USB FIFO to read
 ******************************************************************************/
static void USB_ReadFIFO_Idata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_IDATA), uint8_t fifoNum)
{
  while (--numBytes)
  {
    USB_GetFIFOByte(*dat);
    dat++;
  }
  USB_GetLastFIFOByte(*dat, fifoNum);
}

/***************************************************************************//**
 * @brief       Writes data held in IRAM to the USB FIFO
 * @details     The FIFO to write must be set before calling the function with
 *              @ref USB_EnableWriteFIFO().
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to IDATA buffer holding data to write to the FIFO
 ******************************************************************************/
static void USB_WriteFIFO_Idata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_IDATA))
{
  while (numBytes--)
  {
    USB_SetFIFOByte(*dat);
    dat++;
  }
}

/***************************************************************************//**
 * @brief       Reads data from the USB FIFO to a buffer in XRAM
 * @param       numBytes
 *              Number of bytes to read from the FIFO
 * @param       dat
 *              Pointer to XDATA buffer to hold data read from the FIFO
 * @param       fifoNum
 *              USB FIFO to read
 ******************************************************************************/
static void USB_ReadFIFO_Xdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_XDATA), uint8_t fifoNum)
{
  while (--numBytes)
  {
    USB_GetFIFOByte(*dat);
    dat++;
  }
  USB_GetLastFIFOByte(*dat, fifoNum);
}

/***************************************************************************//**
 * @brief       Writes data held in XRAM to the USB FIFO
 * @details     The FIFO to write must be set before calling the function with
 *              @ref USB_EnableWriteFIFO().
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to XDATA buffer holding data to write to the FIFO
 ******************************************************************************/
static void USB_WriteFIFO_Xdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_XDATA))
{
  while (numBytes--)
  {
    USB_SetFIFOByte(*dat);
    dat++;
  }
}

#if SI_GPTR_MTYPE_PDATA != SI_GPTR_MTYPE_XDATA
/***************************************************************************//**
 * @brief       Reads data from the USB FIFO to a buffer in paged XRAM
 * @param       numBytes
 *              Number of bytes to read from the FIFO
 * @param       dat
 *              Pointer to PDATA buffer to hold data read from the FIFO
 * @param       fifoNum
 *              USB FIFO to read
 ******************************************************************************/
static void USB_ReadFIFO_Pdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_PDATA), uint8_t fifoNum)
{
  while (--numBytes)
  {
    USB_GetFIFOByte(*dat);
    dat++;
  }
  USB_GetLastFIFOByte(*dat, fifoNum);
}

/***************************************************************************//**
 * @brief       Writes data held in paged XRAM to the USB FIFO
 * @details     The FIFO to write must be set before calling the function with
 *              @ref USB_EnableWriteFIFO().
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to PDATA buffer holding data to write to the FIFO
 ******************************************************************************/
static void USB_WriteFIFO_Pdata(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_PDATA))
{
  while (numBytes--)
  {
    USB_SetFIFOByte(*dat);
    dat++;
  }
}

#endif

#if SI_GPTR_MTYPE_DATA != SI_GPTR_MTYPE_IDATA
/***************************************************************************//**
 * @brief       Reads data from the USB FIFO to a buffer in DRAM
 * @param       numBytes
 *              Number of bytes to read from the FIFO
 * @param       dat
 *              Pointer to DATA buffer to hold data read from the FIFO
 * @param       fifoNum
 *              USB FIFO to read
 ******************************************************************************/
static void USB_ReadFIFO_Data(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_DATA), uint8_t fifoNum)
{
  while (--numBytes)
  {
    USB_GetFIFOByte(*dat);
    dat++;
  }
  USB_GetLastFIFOByte(*dat, fifoNum);
}

/***************************************************************************//**
 * @brief       Writes data held in DRAM to the USB FIFO
 * @details     The FIFO to write must be set before calling the function with
 *              @ref USB_EnableWriteFIFO().
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to DATA buffer to hold data read from the FIFO
 ******************************************************************************/
static void USB_WriteFIFO_Data(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_DATA))
{
  while (numBytes--)
  {
    USB_SetFIFOByte(*dat);
    dat++;
  }
}
#endif

/***************************************************************************//**
 * @brief       Writes data held in code space to the USB FIFO
 * @details     The FIFO to write must be set before calling the function with
 *              @ref USB_EnableWriteFIFO().
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to CODE buffer holding data to write to the FIFO
 ******************************************************************************/
static void USB_WriteFIFO_Code(uint8_t numBytes, SI_VARIABLE_SEGMENT_POINTER(dat, uint8_t, SI_SEG_CODE))
{
  while (numBytes--)
  {
    USB_SetFIFOByte(*dat);
    dat++;
  }
}

#else
/***************************************************************************//**
 * @brief       Reads data from the USB FIFO to a buffer in generic memory space
 * @param       numBytes
 *              Number of bytes to read from the FIFO
 * @param       dat
 *              Pointer to generic buffer to hold data read from the FIFO
 * @param       fifoNum
 *              USB FIFO to read
 ******************************************************************************/
static void USB_ReadFIFO_Generic(uint8_t numBytes, uint8_t *dat, uint8_t fifoNum)
{
  while (--numBytes)
  {
    USB_GetFIFOByte(*dat);
    dat++;
  }
  USB_GetLastFIFOByte(*dat, fifoNum);
}

/***************************************************************************//**
 * @brief       Writes data held in generic memory space to the USB FIFO
 * @details     The FIFO to write must be set before calling the function with
 *              @ref USB_EnableWriteFIFO().
 * @param       numBytes
 *              Number of bytes to write to the FIFO
 * @param       dat
 *              Pointer to generic buffer holding data to write to the FIFO
 ******************************************************************************/
static void USB_WriteFIFO_Generic(uint8_t numBytes, uint8_t *dat)
{
  while (numBytes--)
  {
    USB_SetFIFOByte(*dat);
    dat++;
  }
}

#endif  // #ifdef SI_GPTR
