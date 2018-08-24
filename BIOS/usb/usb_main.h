//*----------------------------------------------------------------------------
// USB.H
//*----------------------------------------------------------------------------
#ifndef __USB_H
#define __USB_H
//*----------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "usb_regs.h"
//*----------------------------------------------------------------------------
// Buffer Description Table
//*----------------------------------------------------------------------------
#define BTABLE_ADDRESS            (0x00)
#define EP_CONTROL_RXADDR         (0x20)
#define EP_CONTROL_TXADDR         (0x60)
#define EP_HID_OUT_RXADDR         (0xA0)
#define EP_HID_IN_TXADDR          (0xE0)
//*----------------------------------------------------------------------------
// ENDPOINT DEFINITIONS
//*----------------------------------------------------------------------------
#define EP_CONTROL_SIZE           64

#define EP_HID_IN                 ENDP1
#define EP_HID_OUT                ENDP2
#define EP_HID_SIZE               64
//*----------------------------------------------------------------------------
#define EP_DIR_IN                 0x80
#define EP_DIR_OUT                0x00
//*----------------------------------------------------------------------------
// INTERFACE DEFINITIONS
//*----------------------------------------------------------------------------
#define USB_CLASS                 1
#define USB_HID                   1
#define USB_HID_IF_NUM            0
#define USB_MSC                   0
#define USB_MSC_IF_NUM            0
#define USB_CDC  	    	          0
#define USB_CDC_CIF_NUM           0
#define USB_CDC_DIF_NUM           0
//*----------------------------------------------------------------------------
#define USB_UNICODE(a)                      (a), 0x00
#define USB_STRING_DESCRIPTOR_SIZE(size)    ((size * 2) + 2)

#define USB2_00                             0x0200
#define USB1_10                             0x0110

#define USB_CONFIG_SELF_WAKEUP              0xE0
#define USB_POWER_MA(power)                 (power/2)
//*----------------------------------------------------------------------------
// EP TYPE
//*----------------------------------------------------------------------------
#define EP_TYPE_CTRL                        0
#define EP_TYPE_ISOC                        1
#define EP_TYPE_BULK                        2
#define EP_TYPE_INTR                        3
//*----------------------------------------------------------------------------
/*  Device Status */
//*----------------------------------------------------------------------------
#define USB_STATE_DEFAULT                   0
#define USB_STATE_ADDRESSED                 1
#define USB_STATE_CONFIGURED                2
#define USB_STATE_SUSPENDED                 3
//*----------------------------------------------------------------------------
typedef union {
  uint16_t W;
  struct {
    uint8_t L;
    uint8_t H;
  } __attribute__((packed)) WB;
} __attribute__((packed)) WORD_BYTE;
//*----------------------------------------------------------------------------
// bmRequestType Definition
//*----------------------------------------------------------------------------
typedef union _REQUEST_TYPE {
	struct _BM {
    uint8_t Recipient : 5;
    uint8_t Type      : 2;
    uint8_t Dir       : 1;
  } __attribute__((packed)) BM;
  uint8_t B;
} __attribute__((packed)) REQUEST_TYPE;
//*----------------------------------------------------------------------------
// USB Default Control Pipe Setup Packet 
//*----------------------------------------------------------------------------
typedef struct _USB_SETUP_PACKET {
  REQUEST_TYPE bmRequestType;
  uint8_t      bRequest;
  WORD_BYTE    wValue;
  WORD_BYTE    wIndex;
  uint16_t     wLength;
} __attribute__((packed)) USB_SETUP_PACKET;
//*----------------------------------------------------------------------------
// bmRequestType.Dir
//*----------------------------------------------------------------------------
#define REQUEST_HOST_TO_DEVICE        0
#define REQUEST_DEVICE_TO_HOST        1
//*----------------------------------------------------------------------------
// bmRequestType.Type
//*----------------------------------------------------------------------------
#define USB_REQ_TYPE_STANDARD         0
#define USB_REQ_TYPE_CLASS            1
#define USB_REQ_TYPE_VENDOR           2
#define USB_REQ_TYPE_MASK             3
//*----------------------------------------------------------------------------
// bmRequestType.Recipient
//*----------------------------------------------------------------------------
#define USB_REQ_RECIPIENT_DEVICE      0x00
#define USB_REQ_RECIPIENT_INTERFACE   0x01
#define USB_REQ_RECIPIENT_ENDPOINT    0x02
#define USB_REQ_RECIPIENT_MASK        0x03
//*----------------------------------------------------------------------------
//*----------------------------------------------------------------------------
// USB Standard Request Codes */
//*----------------------------------------------------------------------------
#define USB_REQ_GET_STATUS            0x00
#define USB_REQ_CLEAR_FEATURE         0x01
#define USB_REQ_SET_FEATURE           0x03
#define USB_REQ_SET_ADDRESS           0x05
#define USB_REQ_GET_DESCRIPTOR        0x06
#define USB_REQ_SET_DESCRIPTOR        0x07
#define USB_REQ_GET_CONFIGURATION     0x08
#define USB_REQ_SET_CONFIGURATION     0x09
#define USB_REQ_GET_INTERFACE         0x0A
#define USB_REQ_SET_INTERFACE         0x0B
#define USB_REQ_SYNCH_FRAME           0x0C
//*----------------------------------------------------------------------------
// Descriptor Types
//*----------------------------------------------------------------------------
#define USB_DESC_TYPE_DEVICE                     0x01
#define USB_DESC_TYPE_CONFIGURATION              0x02
#define USB_DESC_TYPE_STRING                     0x03
#define USB_DESC_TYPE_INTERFACE                  0x04
#define USB_DESC_TYPE_ENDPOINT                   0x05
#define USB_DESC_TYPE_DEVICE_QUALIFIER           0x06
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION  0x07
#define USB_DESC_TYPE_IAD                        0x0B
//*----------------------------------------------------------------------------
// Class devices
//*----------------------------------------------------------------------------
#define USB_CLASS_COMMUNICATION         0x02
#define USB_CLASS_HUMAN_INTERFACE       0x03
#define USB_CLASS_STORAGE               0x08
#define USB_CLASS_MISCELLANEOUS         0xEF
//*----------------------------------------------------------------------------
typedef enum {
  WAIT_SETUP,       /* 0 */
  SETTING_UP,       /* 1 */
  IN_DATA,          /* 2 */
  OUT_DATA,         /* 3 */
  LAST_IN_DATA,     /* 4 */
  LAST_OUT_DATA,    /* 5 */
  WAIT_STATUS_IN,   /* 6 */
  WAIT_STATUS_OUT,  /* 7 */
  STALLED,          /* 8 */
  PAUSE             /* 9 */
} CONTROL_STATE;    /* The state machine states of a control pipe */
//*----------------------------------------------------------------------------
typedef struct {
  //........................................................
  USB_SETUP_PACKET Setup;
  u16 SaveRState;
  u16 SaveTState;
  u8  config;
  u8  dev_state;
  u8  dev_old_state;
  //........................................................
  CONTROL_STATE ctrl_state;
  u16 ctrl_len;
  //........................................................
  u8  *io_ptr;
  u16 io_cnt;
  //........................................................
} T_USB;

extern T_USB usb;
//*----------------------------------------------------------------------------
// FUNKTIONEN
//*----------------------------------------------------------------------------
extern void USB_Init(void);
extern void USB_Ctrl_SendStatus(T_USB *pUsb);
extern void USB_Ctrl_Send(T_USB *pUsb, u8 *ptr, u16 len);
//*----------------------------------------------------------------------------

#endif  /* __USB_H__ */
