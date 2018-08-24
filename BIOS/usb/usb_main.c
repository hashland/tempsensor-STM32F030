//*----------------------------------------------------------------------------
// usb_main.c
//*----------------------------------------------------------------------------
#include "sys_config.h"
#include "tools.h"
#include "usb_main.h"
#include "usb_ll.h"
#include "usb_core.h"
#include "usb_desc.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
// GLOBALE USB-Variablen
//*----------------------------------------------------------------------------
T_USB usb;
//*----------------------------------------------------------------------------
WEAK void USBX_StateChange(u8 state);

WEAK void HID_Config(T_USB *pUsb);
WEAK void HID_Ep0Setup(T_USB *pUsb);
WEAK void HID_RxReady(T_USB *pUsb);
WEAK void HID_Out(T_USB *pUsb);
WEAK void HID_In(T_USB *pUsb);
//*----------------------------------------------------------------------------
#define USB_DEBUG
//*----------------------------------------------------------------------------
// USB INIT
//*----------------------------------------------------------------------------
void USB_Init(void)
{
  //==== CORE INIT ====  
  usb.dev_state = USB_STATE_DEFAULT;
  USB_LL_Init();

  //==== INTERRUPT ENABLE ====
  NVIC_SetPriority(USB_IRQn, 2);        // Priority
  NVIC_EnableIRQ(USB_IRQn);             // enable USB interrupt
}
//*----------------------------------------------------------------------------
// SET CONFIGURES
//*----------------------------------------------------------------------------
void USB_SetConfigured(T_USB *pUsb)
{
#ifdef USB_DEBUG  
  dbg("SET CONFIGURED%r");
#endif  
  
  USBX_StateChange(USB_STATE_CONFIG);
  
  //==== HIER CLASSES INITIALISIEREN ====
  HID_Config(pUsb);
}
//*----------------------------------------------------------------------------
// RESET CALLBACK
//*----------------------------------------------------------------------------
void USB_Reset_Callback(T_USB *pUsb)
{
#ifdef USB_DEBUG  
  dbg("RESET CALLBACK%r");
#endif  

  USBX_StateChange(USB_STATE_RESET);

  //==== EP0 SETUP ====
  _SetEPType(ENDP0,     EP_CONTROL);
  _SetEPAddress(ENDP0,  0);
  _SetEPRxAddr(ENDP0,   EP_CONTROL_RXADDR);
  _SetEPTxAddr(ENDP0,   EP_CONTROL_TXADDR);
  _SetEPRxCount(ENDP0,  EP_CONTROL_SIZE);
  _SetEPTxStatus(ENDP0, EP_TX_STALL);
  _SetEPRxStatus(ENDP0, EP_RX_VALID);  
  
  pUsb->dev_state = USB_STATE_DEFAULT;
  pUsb->config    = 0;
}
//*----------------------------------------------------------------------------
// SUSPEND CALLBACK
//*----------------------------------------------------------------------------
void USB_Suspend_Callback(T_USB *pUsb)
{
#ifdef USB_DEBUG  
  dbg("SUSPEND CALLBACK%r");
#endif  

  USBX_StateChange(USB_STATE_SUSPEND);

  pUsb->dev_old_state =  pUsb->dev_state;
  pUsb->dev_state = USB_STATE_SUSPENDED;
}  
//*----------------------------------------------------------------------------
// RESUME CALLBACK
//*----------------------------------------------------------------------------
void USB_Resume_Callback(T_USB *pUsb)
{
#ifdef USB_DEBUG  
  dbg("RESUME CALLBACK%r");
#endif  

  USBX_StateChange(USB_STATE_RESUME);

  pUsb->dev_state = pUsb->dev_old_state;
}
//*----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//*----------------------------------------------------------------------------
// CTRL SEND STATUS
//*----------------------------------------------------------------------------
void USB_Ctrl_SendStatus(T_USB *pUsb)
{
  _SetEPTxCount(ENDP0, 0);
  pUsb->SaveTState = EP_TX_VALID;
  pUsb->ctrl_state = WAIT_STATUS_IN;  
}
//*----------------------------------------------------------------------------
// DATA - IN - HANDLING
//*----------------------------------------------------------------------------
static void USB_DataStageIn(T_USB *pUsb)
{
  CONTROL_STATE cs = pUsb->ctrl_state;
  u16 anz = EP_CONTROL_SIZE;

  if((pUsb->io_cnt == 0) && (cs == LAST_IN_DATA)) {
    cs = WAIT_STATUS_OUT;
    pUsb->SaveTState = EP_TX_STALL;
  } else {
    cs = (pUsb->io_cnt <= anz) ? LAST_IN_DATA : IN_DATA;
    if(anz > pUsb->io_cnt) { anz = pUsb->io_cnt; }
    UserToPMABufferCopy((u8 *)pUsb->io_ptr, EP_CONTROL_TXADDR, anz);
    _SetEPTxCount(ENDP0, anz);
    pUsb->io_cnt -= anz;
    pUsb->io_ptr += anz;
    pUsb->SaveTState = EP_TX_VALID;
    pUsb->SaveRState = EP_RX_VALID;
  }
  pUsb->ctrl_state = cs;
}
//*----------------------------------------------------------------------------
// DATA - OUT - HANDLING
//*----------------------------------------------------------------------------
static void USB_DataStageOut(T_USB *pUsb)
{
  u16 anz = EP_CONTROL_SIZE;

  if(pUsb->io_cnt) {
    if(anz > pUsb->io_cnt) { anz = pUsb->io_cnt; }
    PMAToUserBufferCopy((u8 *)pUsb->io_ptr, EP_CONTROL_RXADDR, anz);
    pUsb->io_cnt -= anz;
    pUsb->io_ptr += anz;
  }
  if(pUsb->io_cnt) {
    pUsb->SaveRState = EP_RX_VALID;
    _SetEPTxCount(ENDP0, 0);
    pUsb->SaveTState = EP_TX_VALID;
  }
  if(pUsb->io_cnt >= EP_CONTROL_SIZE) {
    pUsb->ctrl_state = OUT_DATA;
  } else {
    if(pUsb->io_cnt > 0) {
      pUsb->ctrl_state = LAST_OUT_DATA;
    } else if(pUsb->io_cnt == 0) {
      USB_Ctrl_SendStatus(pUsb);
    }
  }
}
//*----------------------------------------------------------------------------
// POST0 - Stall EP0 in case of Error
//*----------------------------------------------------------------------------
static void USB_Ctrl_PostProcess(T_USB *pUsb)
{
  _SetEPRxCount(ENDP0, EP_CONTROL_SIZE);
  if(pUsb->ctrl_state == STALLED) {
    pUsb->SaveRState = EP_RX_STALL;
    pUsb->SaveTState = EP_TX_STALL;
  }
}
//*----------------------------------------------------------------------------
// EP0 Daten senden
//*----------------------------------------------------------------------------
void USB_Ctrl_Send(T_USB *pUsb, u8 *ptr, u16 len)
{
  pUsb->io_cnt = MIN(len, pUsb->ctrl_len);
  pUsb->io_ptr = ptr;
  USB_DataStageIn(pUsb);
}
//*----------------------------------------------------------------------------
// DEVICE REQUEST
//*----------------------------------------------------------------------------
static void USB_DevReq(T_USB *pUsb)
{
  u16 len = 0;
  u8  *pBuf;
  
  switch(pUsb->Setup.bRequest) {
    //................................................................
    case USB_REQ_GET_DESCRIPTOR: 
      switch(pUsb->Setup.wValue.WB.H) {
        //++++++++++++++++++++++++++++++++++++++++++++++++++
        case USB_DESC_TYPE_DEVICE:
          pBuf = (u8 *)&DevDesc;
          len  = pBuf[0];
          break;
        //++++++++++++++++++++++++++++++++++++++++++++++++++
        case USB_DESC_TYPE_CONFIGURATION:
          pBuf = (u8 *)&ConfDesc;
          len  = pBuf[2];
          break;
        //++++++++++++++++++++++++++++++++++++++++++++++++++
        case USB_DESC_TYPE_STRING:
  		    if((pUsb->Setup.wValue.WB.L) < 4) {
	          pBuf = (uint8_t *)StrDesc[pUsb->Setup.wValue.WB.L];
            len  = pBuf[0];
	        } else {  
            pUsb->ctrl_state = STALLED;
            return;
          }  
          break;
        //++++++++++++++++++++++++++++++++++++++++++++++++++
        default:
          pUsb->ctrl_state = STALLED;
          return;
        //++++++++++++++++++++++++++++++++++++++++++++++++++
      }   
      break;
    //................................................................
    case USB_REQ_SET_CONFIGURATION:  
 	  	pUsb->config = pUsb->Setup.wValue.W;
      pUsb->dev_state = USB_STATE_CONFIGURED;
      USB_SetConfigured(pUsb);
      USB_Ctrl_SendStatus(pUsb);
      break;
    //................................................................
    case USB_REQ_GET_CONFIGURATION:  
      USB_Ctrl_Send(pUsb, (u8 *)&pUsb->config, 1);
      break;
    //................................................................
    case USB_REQ_SET_ADDRESS:
      pUsb->dev_state = USB_STATE_ADDRESSED;
      USB_Ctrl_SendStatus(pUsb);
      break;
    //................................................................
    default:
      pUsb->ctrl_state = STALLED;
      return;
    //................................................................
  }
  if((len != 0) && (pUsb->Setup.wLength != 0)) {
    len = MIN(len, pUsb->Setup.wLength);
    USB_Ctrl_Send(pUsb, pBuf, len);
  }
}
//*----------------------------------------------------------------------------
// EP0 - SETUP
//*----------------------------------------------------------------------------
void USB_EP0_Setup_Callback(T_USB *pUsb)
{
  PMAToUserBufferCopy((u8 *)&pUsb->Setup, EP_CONTROL_RXADDR, 8);
  
//  dbg_dump((u8 *)&pUsb->Setup, 8);

  pUsb->ctrl_state = SETTING_UP;
  pUsb->ctrl_len   = pUsb->Setup.wLength;
  switch(pUsb->Setup.bmRequestType.BM.Recipient) {
    //..........................................................................
    case USB_REQ_RECIPIENT_DEVICE:      //(0)
      USB_DevReq(pUsb);
      break;
    //..........................................................................
    case USB_REQ_RECIPIENT_INTERFACE:   //(1)
      switch(pUsb->Setup.wIndex.WB.L) {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case USB_HID_IF_NUM:
          HID_Ep0Setup(pUsb);
          break;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        default:
          pUsb->ctrl_state = STALLED;
          break;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      }  
      break;  
    //..........................................................................
    case USB_REQ_RECIPIENT_ENDPOINT:    //(2)
      USB_Ctrl_SendStatus(pUsb);
      break;
    //..........................................................................
    default: 
      pUsb->ctrl_state = STALLED;
      break;
    //..........................................................................
  }  
  USB_Ctrl_PostProcess(pUsb);
}
//*----------------------------------------------------------------------------
// EP0 - IN
//*----------------------------------------------------------------------------
void USB_EP0_In_Callback(T_USB *pUsb)
{
  CONTROL_STATE cs = pUsb->ctrl_state;

  switch(cs) {
    //......................................................
    case IN_DATA:
    case LAST_IN_DATA:
      USB_DataStageIn(pUsb);
      cs = pUsb->ctrl_state;
      break;
    //......................................................
    case WAIT_STATUS_IN:
      //==== SET ADDRESS ====
      if(pUsb->Setup.bRequest == USB_REQ_SET_ADDRESS) {
        USB_Set_Address(pUsb->Setup.wValue.WB.L);
        cs = STALLED;
      }
      //==== HID ====
      HID_RxReady(pUsb);
    //......................................................
    default:      
      cs = STALLED;
      break;
    //......................................................
  }
  pUsb->ctrl_state = cs;
  USB_Ctrl_PostProcess(pUsb);
}  
//*----------------------------------------------------------------------------
// EP0 - IN
//*----------------------------------------------------------------------------
void USB_EP0_Out_Callback(T_USB *pUsb)
{
  CONTROL_STATE cs = pUsb->ctrl_state;
  
  switch(cs) {
    //......................................................
    case OUT_DATA:
    case LAST_OUT_DATA:
      USB_DataStageOut(pUsb);
      cs = pUsb->ctrl_state;
      break;
    //......................................................
    default:
      cs = STALLED;
      break;
    //......................................................
  }
  pUsb->ctrl_state = cs;
  USB_Ctrl_PostProcess(pUsb);
}  
//*----------------------------------------------------------------------------
// EPx - OUT
//*----------------------------------------------------------------------------
void USB_Out_Callback(T_USB *pUsb, u8 epnum)
{
  dbg("OUT: %d%r", epnum);
  switch(epnum) {
    //......................................................
    case EP_HID_OUT:
      HID_Out(pUsb);
      break;
    //......................................................
  }      
}  
//*----------------------------------------------------------------------------
// EPx - IN
//*----------------------------------------------------------------------------
void USB_In_Callback(T_USB *pUsb, u8 epnum)
{
//  dbg("IN: %d%r", epnum);
  switch(epnum) {
    //......................................................
    case EP_HID_IN & 0x7F:
      HID_In(pUsb);
      break;
    //......................................................
  }  
}
//*----------------------------------------------------------------------------
