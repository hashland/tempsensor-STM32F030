//*----------------------------------------------------------------------------
// main.c
//*----------------------------------------------------------------------------
#include "tools.h"
#include "cr_section_macros.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
// MAIN VARIABLEN
//*----------------------------------------------------------------------------
T_VAR var;              // Bootloader-Variablen
//*----------------------------------------------------------------------------
// SINGLE PORT SET
//*----------------------------------------------------------------------------
void Port_SetMode(GPIO_TypeDef *port, u8 pnr, GPIOMode_TypeDef mode)
{
  //==== SET MODE ====
  u32 tmp  = port->MODER & ~(0x3 << (pnr * 2));
  tmp |= ((mode & 0x03) << (pnr * 2));
  port->MODER = tmp;
}
//*----------------------------------------------------------------------------
// PORT CONFIG
//*----------------------------------------------------------------------------
void Port_Config(T_PORTCFG *pcfg)
{
  u32 tmp;
  GPIO_TypeDef *p;
  while(pcfg->port) {
    p = pcfg->port;
    
    //==== SET MODE ====
    Port_SetMode(pcfg->port, pcfg->pnr, pcfg->mode);
    //==== SET OUTPUT-TYPE ==== 
    tmp  = p->OTYPER & ~(0x1 << pcfg->pnr);
    tmp |= (pcfg->type << pcfg->pnr);
    p->OTYPER = tmp;
    //==== SET SPEED ====
    tmp  = p->OSPEEDR & ~(0x3 << (pcfg->pnr * 2));
    tmp |= (pcfg->speed << (pcfg->pnr * 2));
    p->OSPEEDR = tmp;
    //==== SET PUPD ====
    tmp  = p->PUPDR & ~(0x3 << (pcfg->pnr * 2));
    tmp |= (pcfg->pupd << (pcfg->pnr * 2));
    p->PUPDR = tmp;
    //==== SET ALT FUNC ====
    tmp = p->AFR[pcfg->pnr / 8] & ~(0xF << ((pcfg->pnr % 8) * 4));
    tmp |= (pcfg->af << ((pcfg->pnr % 8) * 4));
    p->AFR[pcfg->pnr / 8] = tmp;
    
    pcfg++;
  };
}
//*----------------------------------------------------------------------------
// PORT CONFIG
//*----------------------------------------------------------------------------
const T_PORTCFG ports_cfg[] = {
  //==== GPIOA ====
  { GPIOA, ONEWIRE,    GPIO_Mode_OUT, GPIO_OType_OD, GPIO_Speed_Level_2, GPIO_PuPd_UP,     0 },
  { GPIOA, DEBUG_OUT,  GPIO_Mode_AF,  GPIO_OType_PP, GPIO_Speed_Level_2, GPIO_PuPd_UP,     GPIO_AF_1 },
  //==== GPIOF ====
  { GPIOF, LED_G,      GPIO_Mode_OUT, GPIO_OType_PP, GPIO_Speed_Level_2, GPIO_PuPd_UP,     0 },
  { GPIOF, LED_R,      GPIO_Mode_OUT, GPIO_OType_PP, GPIO_Speed_Level_2, GPIO_PuPd_UP,     0 },
  { 0 }  
};
//*----------------------------------------------------------------------------
// PORTS INIT
//*----------------------------------------------------------------------------
void Port_Init(void)
{
  //==== GPIOA ====
  GPIOA->SETPORT = BIT(ONEWIRE);
  
  //==== GPIOF ====
  GPIOF->CLRPORT = BIT(LED_R) | BIT(LED_G);

  Port_Config((T_PORTCFG *)&ports_cfg);
}
//*----------------------------------------------------------------------------
// PERIPHERALS CLOCKS INITIALISIEREN
//*----------------------------------------------------------------------------
void Clock_Init(void)
{
  //==== AHB = 48MHz ====
  RCC->AHBENR   |= RCC_AHBENR_GPIOAEN
                 | RCC_AHBENR_GPIOBEN
                 | RCC_AHBENR_GPIOFEN;

  //==== APB1 = 48MHz ====
  RCC->APB1ENR  |= RCC_APB1ENR_PWREN
                 | RCC_APB1ENR_USBEN
                 | RCC_APB1ENR_CRSEN
                 | RCC_APB1ENR_USART2EN;
     
  //==== APB2 = 48MHz ====
  RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN
                 | RCC_APB2ENR_TIM16EN;
  
  SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP;
  
  //==== MEASURE COUNTER ====
  TIM16->CR1 = 0; 
  TIM16->PSC = 47;          // 1µs
  TIM16->CR1 = TIM_CR1_CEN;
}  
//*----------------------------------------------------------------------------
// COPY MEMORY
//*----------------------------------------------------------------------------
void MemCopy(void *dest, void *src, u32 anz)
{
  u8 *d = (u8 *)dest;
  u8 *s = (u8 *)src;
  while(anz--) { *d++ = *s++; }
}
//*----------------------------------------------------------------------------
// FILL MEMORY
//*----------------------------------------------------------------------------
void MemSet(const void *dest, u8 value, u32 anz)
{
  u8 *d = (u8 *)dest;
  while(anz--) { *d++ = value; }
}
//*----------------------------------------------------------------------------
// CRC - BERECHNUNG
//*----------------------------------------------------------------------------
u32 Crc32(u32 beg, u16 anz)
{
  CRC->CR = CRC_CR_RESET; 
  while(anz) {
    CRC->DR = *(__IO uint32_t *)beg;
    anz -= 4;
    beg += 4;
  }  
  return CRC->DR;
}
//*----------------------------------------------------------------------------
// FLASH WRITE (anz = 32Bit Worte)
//*----------------------------------------------------------------------------
u32 FLASH_Write(u32 adr, u32 *ptr, u16 anz)
{
  u32 rc = 0; 
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
  while(anz) {
    if((adr & 0x000003FF) == 0) {
      rc = FLASH_ErasePage(adr);
      if(rc != FLASH_COMPLETE) { goto ende; }
    }
    rc = FLASH_ProgramWord(adr, *ptr);
    if(rc != FLASH_COMPLETE) { goto ende; }
    adr += 4;
    ptr++;               
    anz--;
  }

ende:
  FLASH_Lock();
  return rc;
}
//*----------------------------------------------------------------------------
// SYSTEM INIT 48MHz SYS + USB
//*----------------------------------------------------------------------------
void System_Init(void) 
{ 
/*  
  //==== Starte externen Oszillator ====
  RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR & RCC_CR_HSERDY));

  //==== PLL konfigurieren ====
  RCC->CFGR &= ~(RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC);
  RCC->CFGR |= (uint32_t)(RCC_PLLSource_HSE | RCC_PLLMul_6);
  //==== Enable PLL ====
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY));
*/
  //==== Configure Flash =====
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
  
  //==== HSI48 OSCILLATOR ENABLE ====
  RCC->CR2 = RCC_CR2_HSI48ON; 
  while((RCC->CR2 & RCC_CR2_HSI48RDY) == 0);
  /* Select HSI48L as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_SYSCLKSource_HSI48;
  /* Wait till HSI48 is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_HSI48);

  
/*
  //==== Select PLL as system clock source ====
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_SYSCLKSource_PLLCLK;
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL);
  
  //==== Select PLL as USB clock ====
  RCC->CFGR3 |= RCC_CFGR3_USBSW;
*/
  SystemCoreClock = 48000000;
}
//*----------------------------------------------------------------------------
// DELAY (us)
//*----------------------------------------------------------------------------
void delxus(u16 us)
{
  TIM16->CNT = 0;
  while(TIM16->CNT < us);
}
//*----------------------------------------------------------------------------
// BOOT-INFO
//*----------------------------------------------------------------------------
__SECTION(bootinfo, 0) 
const T_BOOTINFO bootinfo = {
  .SysId      = SYSID,  
  .VersHi     = BIOS_VER_MAJOR,
  .VersLo     = BIOS_VER_MINOR,
  .UpdateVers = 0,
  .Value_8    = 0,
  .BiosCode   = BIOSCODE,
  .BiosLoad   = 0,
  .Date       = __DATE__,
  .Snr        = "22334-55667-889",
}; 
//*----------------------------------------------------------------------------
