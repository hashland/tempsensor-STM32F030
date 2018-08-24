//*----------------------------------------------------------------------------
// onewire.c
//*----------------------------------------------------------------------------
#include "onewire.h"
#include "tools.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
#define DATA_HIGH() GPIOA->SETPORT=BIT(ONEWIRE)
#define DATA_LOW()  GPIOA->CLRPORT=BIT(ONEWIRE)
#define DATA_IN()   (GPIOA->IDR & BIT(ONEWIRE))
//*----------------------------------------------------------------------------
// ONEWIRE INIT
//*----------------------------------------------------------------------------
void OW_Init(void)
{
  DATA_HIGH();
  delxus(1000);
}
//*----------------------------------------------------------------------------
// onewire_reset () - reset all slaves, check if one slave is connected
// Timing:
//   Master: Holds Bus min. 480us low, waits 15-60us
//   Slave shows presence when it pulls bus low in the next 60us for 60-240us
//   Master waits 480us (pulse above) + 480us presence detection following
//*----------------------------------------------------------------------------
int OW_Reset(void)
{
  int err;

  DATA_LOW();
  delxus(480);
  DATA_HIGH();
  delxus(64);
  err  = DATA_IN();
  delxus(480 - 64);
  if(DATA_IN() == 0) { err = 1; }
  return err; 
}
//*----------------------------------------------------------------------------
// onewire_read_bit () - read 1 bit
// Timing:
//    Master: 1 - 15 us Low
//    Slave:  Holds Bus Low if bit is 0
//*----------------------------------------------------------------------------
static u8 ow_read_bit(void)
{
  u8 res;
  DATA_LOW();
  delxus(5);
  DATA_HIGH();
  delxus(10);
  res = GPIOA->IDR & BIT(ONEWIRE);
  delxus(50);
  return res;
}
//*----------------------------------------------------------------------------
// IO-BIT
//*----------------------------------------------------------------------------
u8 OW_IoBit(u8 b)
{
  DATA_LOW();
  delxus(2);
  if(b) { DATA_HIGH(); }  
  delxus(15 - 2);
  if(DATA_IN() == 0) { b = 0; }
  delxus(60 - 15 - 2);
  DATA_HIGH();
  delxus(OW_RECOVERY_TIME);
  return b;
}
//*----------------------------------------------------------------------------
// onewire_read_byte () - read 1 byte
//*----------------------------------------------------------------------------
u8 OW_ReadByte(void)
{
  u8 res  = 0;

  for(u8 mask = 0x01; mask; mask <<= 1) {
    if(ow_read_bit()) { res |= mask; }
  }
  return res;
}
//*----------------------------------------------------------------------------
// onewire_read () - read n bytes
//*----------------------------------------------------------------------------
void OW_nRead(u8 *ptr, u8 len)
{
  while(len--) { *ptr++ = OW_ReadByte(); }
}
//*----------------------------------------------------------------------------
// onewire_write_byte () - write 1 byte
// Timing:
//    1:    Low:  1 -  15us
//    0:    Low: 60 - 120us
//*----------------------------------------------------------------------------
void OW_WriteByte(u8 value)
{
  for(u8 mask = 0x01; mask; mask <<= 1) {
    if(value & mask) {
      DATA_LOW();                     // write high bit
      delxus(5);
      DATA_HIGH();
      delxus(80);
    } else {
      DATA_LOW();                     // write low bit
      delxus(80);
      DATA_HIGH();
      delxus(5);
    }
  }  
}
//*----------------------------------------------------------------------------
// onewire_write () - write n bytes
//*----------------------------------------------------------------------------
void OW_nWrite(u8 *ptr, u8 len)
{
  while(len--) { OW_WriteByte(*ptr++); }
}
//*----------------------------------------------------------------------------
// READ PIN STATE
//*----------------------------------------------------------------------------
u8 OW_PinState(void)
{
  return DATA_IN(); 
}
//*----------------------------------------------------------------------------
// ONEWIRE ROM SEARCH
//*----------------------------------------------------------------------------
u8 OW_RomSearch(u8 diff, u8 *id)
{
  u8 i, j, b, next_diff = OW_LAST_DEVICE;
  
  if(OW_Reset()) { return OW_PRESENCE_ERR; }
  OW_WriteByte(OW_SEARCH_ROM);  
  
  i = OW_ROMCODE_SIZE * 8;
  do {
    j = 8;
    do {
      b = OW_IoBit(1);
      if(OW_IoBit(1)) {
        if(b) { return OW_DATA_ERR; } 
      } else {
        if(!b) {
          if((diff > i) || ((*id & 1) && (diff != i))) {
            b = 1; 
            next_diff = i; 
          }  
        }  
      } 
      OW_IoBit(b);
      *id >>= 1;
      if(b) { *id |= 0x80; }
      i--;   
    } while(--j);
    id++;
  } while(i);

  return next_diff;  
}
//*----------------------------------------------------------------------------
// ONEWIRE COMMAND
//*----------------------------------------------------------------------------
void OW_Command(u8 command, u8 *id)
{
  OW_Reset();
  if(id) {
    OW_WriteByte(OW_MATCH_ROM);
    OW_nWrite(id, OW_ROMCODE_SIZE);
  } else {
    OW_WriteByte(OW_SKIP_ROM);   
  }  
  OW_WriteByte(command);    
}  
//*----------------------------------------------------------------------------
