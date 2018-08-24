//*----------------------------------------------------------------------------
// usb_ll.c
//*----------------------------------------------------------------------------
#include "usb_ll.h"
#include "usb_main.h"
#include "usb_core.h"
#include "sys_config.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
// GLOBALE USB-Variablen
//*----------------------------------------------------------------------------
T_USB usb;
//*----------------------------------------------------------------------------
#define CORE_DEBU__G
//*----------------------------------------------------------------------------
WEAK void USB_Suspend_Callback(T_USB *pUsb);
WEAK void USB_Reset_Callback(T_USB *pUsb);
WEAK void USB_Resume_Callback(T_USB *pUsb);
WEAK void USB_Class_Action(T_USB *pUsb);
WEAK void USB_EP0_Setup_Callback(T_USB *pUsb);
WEAK void USB_EP0_Out_Callback(T_USB *pUsb);
WEAK void USB_EP0_In_Callback(T_USB *pUsb);
WEAK void USB_Out_Callback(T_USB *pUsb, u8 epnum);
WEAK void USB_In_Callback(T_USB *pUsb, u8 epnum);
//*----------------------------------------------------------------------------
// USB INIT
//*----------------------------------------------------------------------------
void USB_LL_Init(void)
{
  _SetCNTR(CNTR_FRES);                  // PWR-DOWN OFF, RESET
  _SetBTABLE(BTABLE_ADDRESS);           // BTABLE
  _SetISTR(0);                          // Pending Interrupts clear
  _SetCNTR(CNTR_CTRM | CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM);
  _SetBCDR(BCDR_DPPU);                  // Pullup Enable
}
//*----------------------------------------------------------------------------
// SET DEVICE ADDRESS
//*----------------------------------------------------------------------------
void USB_Set_Address(u8 address)
{
  _SetDADDR(DADDR_EF | address);
}
//*----------------------------------------------------------------------------
// USB-INTERRUPT-ROUTINE
//*----------------------------------------------------------------------------
void USB_IRQHandler(void)
{
  u16 wIstr = _GetISTR();
  u16 EPindex;
  u16 wEPVal = 0;

  // === CORRECT TRANSFER INTERRUPT ===
  while((wIstr = _GetISTR()) & ISTR_CTR) {
    EPindex = (u8)(wIstr & ISTR_EP_ID);
    wEPVal  = _GetENDPOINT(EPindex);
    
    switch(EPindex) {
      //..............................................................
      case 0:     // Endpoint 0
        usb.SaveRState = _GetEPRxStatus(ENDP0);
        usb.SaveTState = _GetEPTxStatus(ENDP0);
        _SetEPRxStatus(ENDP0, EP_RX_NAK);
        _SetEPTxStatus(ENDP0, EP_TX_NAK);
        if(wEPVal & EP_SETUP) {           // SETUP
          _ClearEP_CTR_RX(ENDP0);
          USB_EP0_Setup_Callback(&usb);
        } else if(wEPVal & EP_CTR_RX) {   // OUT
          _ClearEP_CTR_RX(ENDP0);
          USB_EP0_Out_Callback(&usb);
        } else if(wEPVal & EP_CTR_TX) {   // IN
          _ClearEP_CTR_TX(ENDP0);
          USB_EP0_In_Callback(&usb);
        }
        _SetEPRxStatus(ENDP0, usb.SaveRState);
        _SetEPTxStatus(ENDP0, usb.SaveTState);
        break;
      //............................................................
      default:  
        if(wEPVal & EP_CTR_RX) {   // OUT
          _ClearEP_CTR_RX(EPindex);
          USB_Out_Callback(&usb, EPindex);           
        }  
        if(wEPVal & EP_CTR_TX) {   // IN
          _ClearEP_CTR_TX(EPindex);
          USB_In_Callback(&usb, EPindex);        
        }  
        break;
      //............................................................
    }  
  }  

  //=== RESET ===
  if(wIstr & ISTR_RESET) {
    _SetISTR(CLR_RESET);
    _SetCNTR(_GetCNTR() & ~(CNTR_FSUSP));
    USB_Set_Address(0);
    USB_Reset_Callback(&usb);
  }
  // === SUSPEND INTERRUPT ===
  if(wIstr & ISTR_SUSP) {
    _SetISTR(CLR_SUSP);
    _SetCNTR(_GetCNTR() | CNTR_FSUSP);
    _SetCNTR(_GetCNTR() | CNTR_LPMODE);
    USB_Suspend_Callback(&usb);
  }
  // === WAKEUP/RESUME ===
  if(wIstr & ISTR_WKUP) {
    _SetCNTR(_GetCNTR() & ~(CNTR_LPMODE));
    _SetCNTR(_GetCNTR() & ~(CNTR_FSUSP));
    USB_Resume_Callback(&usb);
    _SetISTR(CLR_WKUP);
  }
}
//*----------------------------------------------------------------------------
