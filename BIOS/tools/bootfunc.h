//*----------------------------------------------------------------------------
// bootfunc.h
//*----------------------------------------------------------------------------
#ifndef __BOOTFUNC_H
#define __BOOTFUNC_H
//*----------------------------------------------------------------------------
#include "stm32f0xx.h"
//*----------------------------------------------------------------------------
// CONSTANTS
//*----------------------------------------------------------------------------
#define BUF_SIZE              268  
//*----------------------------------------------------------------------------
#define MESSAGE_START         0x1B
#define TOKEN                 0x0E
//*----------------------------------------------------------------------------
// RESET SYSTEM
#define BOOT_SET_RESET        0xF1

// Always executable
#define BOOT_GET_BOOTINFO     0xF2
#define BOOT_GET_BIOSINFO     0xF3
#define BOOT_GET_CPUINFO      0xF5
#define BOOT_READ_MEM         0xF6
#define BOOT_UPLOAD_BUF       0xF7

// Only in Boot-Mode
#define BOOT_WRITE_FLASH      0xF9
//*----------------------------------------------------------------------------
// MEMORY-TYPE
//*----------------------------------------------------------------------------
#define MEMORY_FLASH          0
#define MEMORY_RAM            1
#define MEMORY_EEPROM         2
//*----------------------------------------------------------------------------
// PROTOCOL STATE-MACHINE
//*----------------------------------------------------------------------------
enum msg_state {
  MSG_START,
  MSG_SEQ_NUM,
  MSG_SIZE_LO,
  MSG_SIZE_HI,
  MSG_TOKEN,
  MSG_COMMAND,
  MSG_DATA,
  MSG_CHECK,
  MSG_EXEC
};
//*----------------------------------------------------------------------------
// IN-Message Struktur
//*----------------------------------------------------------------------------
struct __attribute__ ((packed)) msg_struct {
  u8  start_byte;           // Startbyte 0x1B
  u8  seq_num;              // Sequence_number
  u16 byte_cnt;             // Anzahl
  u8  com_status;           // Command/Status
  u8  token_byte;           // Token 0x0E
  union {
    u8  B[BUF_SIZE];        
    u16 W[BUF_SIZE / 2];
    u32 L[BUF_SIZE / 4];
  } data;  
};
//*----------------------------------------------------------------------------
// FUNKTIONSPROTOTYPEN
//*----------------------------------------------------------------------------
extern void BootFunc_Init(void);
extern void BootFunc_Data(u8 *ptr, u8 cnt);
//*----------------------------------------------------------------------------
#endif
