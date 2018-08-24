//*----------------------------------------------------------------------------
// BOARD.H
//*----------------------------------------------------------------------------
#ifndef __BOARD_H
#define __BOARD_H
//*----------------------------------------------------------------------------
#include "stm32f0xx.h"
//*----------------------------------------------------------------------------
// DEFINES
//*----------------------------------------------------------------------------
#define BIOS_VER_MAJOR          1
#define BIOS_VER_MINOR          0
//*----------------------------------------------------------------------------
#define USB_VID                 0x16C0
#define USB_PID                 0x0480
#define SYSID                   0x38334645    // "EF38"
#define BIOSCODE                0x4410        // 4410
//*----------------------------------------------------------------------------
// STM32L042C6
// 32k FLASH, 6k RAM 
//*----------------------------------------------------------------------------
#define FLASHSTART    0x08000000
#define FLASHEND      0x08007FFF
#define FLASHSIZE     0x8000
//*----------------------------------------------------------------------------
#define RAMSTART      0x20000000
#define RAMEND        0x200017FF
#define RAMSIZE       0x1800
//*----------------------------------------------------------------------------
// DEFINITIONEN
//*----------------------------------------------------------------------------
#define ATTR_ALIGNED(Bytes) __attribute__ ((aligned(Bytes)))
#define NAKED               __attribute__ ((naked))
#define PACKED              __attribute__ ((packed))
#define WEAK                __attribute__ ((weak))
#define BIT(n)          (1 << n)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define true  1
#define false 0
typedef u8 bool;
#ifndef NULL
  #define NULL 0
#endif  
//*----------------------------------------------------------------------------
// ALIASES
//*----------------------------------------------------------------------------
#define SETPORT  BSRR 
#define CLRPORT  BRR
//*----------------------------------------------------------------------------
// USB-STATE FÜR MAIN
//*----------------------------------------------------------------------------
enum usb_state {
  USB_STATE_NONE    = 0,
  USB_STATE_SUSPEND = 1,
  USB_STATE_RESET   = 2,
  USB_STATE_RESUME  = 3,
  USB_STATE_CONFIG  = 4
};
//*----------------------------------------------------------------------------
// PORT-KONFIG STRUCT
//*----------------------------------------------------------------------------
typedef struct PACKED {
  GPIO_TypeDef *port;       // PORT: A..E
  u8 pnr;                   // PORT: 0..15
  GPIOMode_TypeDef mode;    // MODE: GPIO_Mode_IN,OUT,AF,AN
  GPIOOType_TypeDef type;   // TYPE: GPIO_OType_PP,OD
  GPIOSpeed_TypeDef speed;  // SPEED: GPIO_Speed_2,25,50,100MHz
  GPIOPuPd_TypeDef pupd;    // PUPD:  GPIO_PuPd_NOPULL,UP,DOWN
  u8 af;                    // Alternate Func
} T_PORTCFG;
//*----------------------------------------------------------------------------
// BOOTINFO AN ADRESSE 0x080000D0
//*----------------------------------------------------------------------------
typedef struct PACKED {
  u32  SysId;         // System-ID
  u8   VersHi;        // Version HI
  u8   VersLo;        // Version LO
  u8   UpdateVers;    // Update-Version
  u8   Value_8;       // Value 8 Bit
  u32  BiosCode;      // BiosCode
  u32  BiosLoad;      // Bios-Load Adresse
  char Date[16];      // Datum
  char Snr[16];       // Seriennummer
} T_BOOTINFO;

extern const T_BOOTINFO bootinfo;
//*----------------------------------------------------------------------------
// PORT-DEFINITIONEN
//*----------------------------------------------------------------------------
// === PORT A ===
#define ONEWIRE           0
#define DEBUG_OUT         2
#define USB_DM            11
#define USB_DP            12
#define SWDIO             13
#define SWCLK             14

// === PORT F ===
#define LED_R             0
#define LED_G             1

//*----------------------------------------------------------------------------
#endif
