//*----------------------------------------------------------------------------
// onewire.h
//*----------------------------------------------------------------------------
#ifndef __ONEWIRE_H
#define __ONEWIRE_H
//*----------------------------------------------------------------------------
#include "tools.h"
//*----------------------------------------------------------------------------
// One-Wire ROM codes
//*----------------------------------------------------------------------------
#define OW_MATCH_ROM        0x55
#define OW_SKIP_ROM         0xCC
#define OW_SEARCH_ROM       0xF0

#define OW_SEARCH_FIRST     0xFF        // start new search
#define OW_PRESENCE_ERR     0xFF
#define OW_DATA_ERR         0xFE
#define OW_LAST_DEVICE      0x00        // last device found

#define OW_ROMCODE_SIZE     8
#define OW_RECOVERY_TIME    10          // usec
//*----------------------------------------------------------------------------
// FUNKTIONEN
//*----------------------------------------------------------------------------
extern void OW_Init(void);
extern int  OW_Reset(void);
extern u8   OW_PinState(void);
extern u8   OW_IoBit(u8 b);
extern u8   OW_ReadByte(void);
extern void OW_nRead(u8 *ptr, u8 len);
extern void OW_WriteByte(u8 value);
extern void OW_nWrite(u8 *ptr, u8 len);
extern u8   OW_RomSearch(u8 diff, u8 *id);
extern void OW_Command(u8 command, u8 *id);
//*----------------------------------------------------------------------------
#endif
