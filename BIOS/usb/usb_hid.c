//*----------------------------------------------------------------------------
// usb_hid.c
//*----------------------------------------------------------------------------
#include "usb_hid.h"
#include "usb_desc.h"
#include "usb_core.h"
#include "bootfunc.h"
#include "tools.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
struct {
  u8 buf[EP_CONTROL_SIZE];
} hid;  
//*----------------------------------------------------------------------------
// HID CONFIG INIT
//*----------------------------------------------------------------------------
void HID_Config(T_USB *pUsb)
{
    //==== HID-OUT ====
  _SetEPType(EP_HID_OUT,        EP_INTERRUPT);
  _SetEPAddress(EP_HID_OUT,     EP_HID_OUT);
  _SetEPRxAddr(EP_HID_OUT,      EP_HID_OUT_RXADDR);
  _SetEPRxCount(EP_HID_OUT,     EP_HID_SIZE);
  _SetEPTxStatus(EP_HID_OUT,    EP_TX_NAK);
  _SetEPRxStatus(EP_HID_OUT,    EP_RX_VALID);  
  
  //==== HID-IN ====
  _SetEPType(EP_HID_IN,         EP_INTERRUPT);
  _SetEPAddress(EP_HID_IN,      EP_HID_IN);
  _SetEPTxAddr(EP_HID_IN,       EP_HID_IN_TXADDR);
  _SetEPTxStatus(EP_HID_IN,     EP_TX_NAK);
  _SetEPRxStatus(EP_HID_IN,     EP_RX_DIS);  
}
//*----------------------------------------------------------------------------
// HID SETUP
//*----------------------------------------------------------------------------
void HID_Ep0Setup(T_USB *pUsb)
{
  switch(pUsb->Setup.bRequest) {
	  //................................................................
    case HID_REQUEST_GET_REPORT:
      switch(pUsb->Setup.wValue.WB.L) {
      }  
      USB_Ctrl_Send(pUsb, (u8 *)&hid.buf, EP_CONTROL_SIZE);
      break;
	  //................................................................
    case HID_REQUEST_SET_REPORT:
  	  pUsb->io_cnt       = pUsb->Setup.wLength;
  	  pUsb->io_ptr       = (u8 *)&hid.buf;
      pUsb->ctrl_state   = OUT_DATA;
      pUsb->SaveRState   = EP_RX_VALID;
      break;
    //++++++++++++++++++++++++++++++++++++++++++++++++++
    case USB_REQ_GET_DESCRIPTOR:
      USB_Ctrl_Send(pUsb, (u8 *)&HID_ReportDescriptor, HID_ReportDescSize);
      break;      
	  //................................................................
    case HID_REQUEST_SET_IDLE:
      USB_Ctrl_SendStatus(pUsb);
      break;
	  //................................................................
    default:
      pUsb->ctrl_state = STALLED;
      break;      
	  //................................................................
  }
}
//*----------------------------------------------------------------------------
// HID SET-REPORT
//*----------------------------------------------------------------------------
void HID_RxReady(T_USB *pUsb)
{
  switch(pUsb->Setup.wValue.WB.L) {
  }  
}  
//*----------------------------------------------------------------------------
