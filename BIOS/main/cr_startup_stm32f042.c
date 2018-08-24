//*****************************************************************************
//   +--+
//   | ++----+
//   +-++    |
//     |     |
//   +-+--+  |
//   | +--+--+
//   +----+    Copyright (c) 2011 Code Red Technologies Ltd.
//
// Microcontroller Startup code for use with Red Suite
//
// Version : 110929
//
// Software License Agreement
//
// The software is owned by Code Red Technologies and/or its suppliers, and is
// protected under applicable copyright laws.  All rights are reserved.  Any
// use in violation of the foregoing restrictions may subject the user to criminal
// sanctions under applicable laws, as well as to civil liability for the breach
// of the terms and conditions of this license.
//
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD.
//
//*****************************************************************************
#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

// Code Red - if CMSIS is being used, then SystemInit() routine
// will be called by startup code rather than in application's main()
#if defined (__USE_CMSIS)
#include "stm32f0xx.h"
#endif
//*****************************************************************************
//
// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions
//
//*****************************************************************************
     void ResetISR(void);
WEAK void NMI_Handler(void) ALIAS(IntDefaultHandler);
WEAK void HardFault_Handler(void) ALIAS(IntDefaultHandler);
WEAK void SVCall_Handler(void) ALIAS(IntDefaultHandler); 
WEAK void DebugMon_Handler(void) ALIAS(IntDefaultHandler);
WEAK void PendSV_Handler(void) ALIAS(IntDefaultHandler);
WEAK void SysTick_Handler(void) ALIAS(IntDefaultHandler);
WEAK void IntDefaultHandler(void);
//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//
//*****************************************************************************
void WWDG_IRQHandler(void) ALIAS(IntDefaultHandler);
void PVD_VDDIO2_IRQHandler(void) ALIAS(IntDefaultHandler);
void RTC_IRQHandler(void) ALIAS(IntDefaultHandler);
void FLASH_IRQHandler(void) ALIAS(IntDefaultHandler);
void RCC_CRS_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI0_1_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI2_3_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI4_15_IRQHandler(void) ALIAS(IntDefaultHandler);
void TSC_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMA1_Channel1_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMA1_Channel2_3_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMA1_Channel4_5_IRQHandler(void) ALIAS(IntDefaultHandler);
void ADC1_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM1_BRK_UP_TRG_COM_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM1_CC_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM2_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM3_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM14_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM16_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM17_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C1_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI1_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI2_IRQHandler(void) ALIAS(IntDefaultHandler);
void USART1_IRQHandler(void) ALIAS(IntDefaultHandler);
void USART2_IRQHandler(void) ALIAS(IntDefaultHandler);
void CEC_CAN_IRQHandler(void) ALIAS(IntDefaultHandler);
void USB_IRQHandler(void) ALIAS(IntDefaultHandler);
//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//
//*****************************************************************************
extern int main(void);
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern void _vStackTop(void);
//*****************************************************************************
//
// The vector table.
// This relies on the linker script to place at correct location in memory.
//
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
	// Core Level - CM3
	&_vStackTop,            // The initial stack pointer
	ResetISR,								// The reset handler
	NMI_Handler,						// The NMI handler
	HardFault_Handler,	    // The hard fault handler
	0,						          // Reserved
	0,						          // Reserved
	0,						          // Reserved
	0,										  // Reserved
	0,										  // Reserved
	0,										  // Reserved
	0,										  // Reserved
	SVCall_Handler,					// SVCall handler
	0,										  // Reserved
	0,										  // Reserved
	PendSV_Handler,					// The PendSV handler
	SysTick_Handler,				// The SysTick handler

	// Chip Level - STM32F042
  WWDG_IRQHandler,                  	// Window WatchDog
  PVD_VDDIO2_IRQHandler,             	// PVD and VDDIO2
  RTC_IRQHandler,              	      // RTC through the EXTI line
  FLASH_IRQHandler,                 	// FLASH
  RCC_CRS_IRQHandler,               	// RCC
  EXTI0_1_IRQHandler,               	// EXTI Line0 and 1
  EXTI2_3_IRQHandler,                 // EXTI Line2 and 3
  EXTI4_15_IRQHandler,                // EXTI Line4 to 15
  TSC_IRQHandler,                 	  // TSC
  DMA1_Channel1_IRQHandler,           // DMA1 Channel 1
  DMA1_Channel2_3_IRQHandler,         // DMA1 Channel 2 and 3
  DMA1_Channel4_5_IRQHandler,         // DMA1 Channel 4 and 5 
  ADC1_IRQHandler,              	    // ADC1
  TIM1_BRK_UP_TRG_COM_IRQHandler,     // TIM1
  TIM1_CC_IRQHandler,                 // TIM1 Capture
  TIM2_IRQHandler,                  	// TIM2
  TIM3_IRQHandler,                  	// TIM3
  0,                                  // Reserved
  0,                                  // Reserved
  TIM14_IRQHandler,                  	// TIM14
  0,                                  // Reserved
  TIM16_IRQHandler,                 	// TIM16
  TIM17_IRQHandler,                 	// TIM17
  I2C1_IRQHandler,               	    // I2C1
  0,                                  // Reserved
  SPI1_IRQHandler,                  	// SPI1
  SPI2_IRQHandler,                  	// SPI2
  USART1_IRQHandler,                	// USART1
  USART2_IRQHandler,                	// USART2
  0,                                  // Reserved
  CEC_CAN_IRQHandler,            	    // CEC + CAN
  USB_IRQHandler,                	    // USB
};
//*****************************************************************************
extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;
//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void ResetISR(void) 
{
	unsigned int *LoadAddr, *StartAddr, *EndAddr;
  // Copy the data segment from flash to SRAM.
	LoadAddr   = &_etext;
	StartAddr  = &_data;
	EndAddr    = &_edata;
	while(StartAddr < EndAddr) { *StartAddr++ = *LoadAddr++; }  

	// Zero fill the bss segment
	StartAddr  = &_bss;
	EndAddr    = &_ebss;
	while(StartAddr < EndAddr) { *StartAddr++ = 0; }  

	main();
	while (1);
} 
//*----------------------------------------------------------------------------
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//*----------------------------------------------------------------------------
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
  while(1) {}
}
//*----------------------------------------------------------------------------
