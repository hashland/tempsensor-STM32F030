//*----------------------------------------------------------------------------
// tools.h
//*----------------------------------------------------------------------------
#ifndef __TOOLS_H
#define __TOOLS_H
//*----------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "sys_config.h"
//*----------------------------------------------------------------------------
// DEFINES
//*----------------------------------------------------------------------------
#define MAXSENSORS    8

//*----------------------------------------------------------------------------
// MAIN STATE-MACHINE
//*----------------------------------------------------------------------------
#define LED_OFF()   GPIOA->SETPORT=BIT(LED_R)|BIT(LED_G)
#define LED_ON()    GPIOA->CLRPORT=BIT(LED_R)|BIT(LED_G)
//*----------------------------------------------------------------------------
// BOOT VARIABLEN
//*----------------------------------------------------------------------------
typedef struct {
  //........................................................
  vu32 sys_ticks;           // SYSTICK counter
  vu32 usb_state;           // USB-State
  //........................................................
} T_VAR;

extern T_VAR var;
//*----------------------------------------------------------------------------
// FUNKTIONSPROTOTYPEN
//*----------------------------------------------------------------------------
u32 clock_time(void);
//*----------------------------------------------------------------------------
extern void System_Init(void);
extern void Port_Init(void);
extern void Clock_Init(void);
extern void MEM_Init(void);
extern void Port_SetMode(GPIO_TypeDef *port, u8 pnr, GPIOMode_TypeDef mode);
extern void MemCopy(void *dest, void *src, u32 anz);
extern void MemSet(const void *dest, u8 value, u32 anz);
extern u32  Crc32(u32 beg, u16 anz);
extern u32  FLASH_Write(u32 adr, u32 *ptr, u16 anz);

extern void delxus(u16 us);
//*----------------------------------------------------------------------------
#endif
