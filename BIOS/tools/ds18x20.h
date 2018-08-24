//*----------------------------------------------------------------------------
// ds18x20.h
//*----------------------------------------------------------------------------
#ifndef __DS18X20_H
#define __DS18X20_H
//*----------------------------------------------------------------------------
#include "tools.h"
#include "onewire.h"
//*----------------------------------------------------------------------------
// return values
//*----------------------------------------------------------------------------
#define DS18X20_OK                  0x00
#define DS18X20_ERROR               0x01
#define DS18X20_START_FAIL          0x02
#define DS18X20_ERROR_CRC           0x03

#define DS18X20_POWER_PARASITE      0x00
#define DS18X20_POWER_EXTERN        0x01

#define DS18X20_CONVERSION_DONE     0x00
#define DS18X20_CONVERTING          0x01

#define DS18X20_INVALID_DECICELSIUS 2000
//*----------------------------------------------------------------------------
// DS18X20 specific values (see datasheet)
//*----------------------------------------------------------------------------
#define DS18S20_FAMILY_CODE         0x10
#define DS18B20_FAMILY_CODE         0x28
#define DS1822_FAMILY_CODE          0x22

#define DS18X20_CONVERT_T           0x44
#define DS18X20_READ                0xBE
#define DS18X20_WRITE               0x4E
#define DS18X20_EE_WRITE            0x48
#define DS18X20_EE_RECALL           0xB8
#define DS18X20_READ_POWER_SUPPLY   0xB4

#define DS18B20_CONF_REG            4
#define DS18B20_9_BIT               0
#define DS18B20_10_BIT              (1<<5)
#define DS18B20_11_BIT              (1<<6)
#define DS18B20_12_BIT              ((1<<6)|(1<<5))
#define DS18B20_RES_MASK            ((1<<6)|(1<<5))
//*----------------------------------------------------------------------------
// undefined bits in LSB if 18B20 != 12bit
//*----------------------------------------------------------------------------
#define DS18B20_9_BIT_UNDF          ((1<<0)|(1<<1)|(1<<2))
#define DS18B20_10_BIT_UNDF         ((1<<0)|(1<<1))
#define DS18B20_11_BIT_UNDF         ((1<<0))
#define DS18B20_12_BIT_UNDF         0
//*----------------------------------------------------------------------------
#define DS18X20_SP_SIZE             9
//*----------------------------------------------------------------------------
// DS18X20 EEPROM-Support
#define DS18X20_WRITE_SCRATCHPAD    0x4E
#define DS18X20_COPY_SCRATCHPAD     0x48
#define DS18X20_RECALL_E2           0xB8
#define DS18X20_COPYSP_DELAY        10 /* ms */
#define DS18X20_TH_REG              2
#define DS18X20_TL_REG              3
#define DS18X20_DECIMAL_CHAR        '.'
//*----------------------------------------------------------------------------
typedef struct PACKED {
  u8  SensCnt;   // Anzahl Sensoren
  u8  SensNr;    // Aktueller Sensor
  u8  Power;     // Extern/Parasite
  u8  Dummy1;
  s16 Temp;       // Temperatur
  u16 Dummy2;
  u8 SensId[OW_ROMCODE_SIZE]; // Sensor-Id
} T_TEMP;
//*----------------------------------------------------------------------------
// FUNKTIONEN
//*----------------------------------------------------------------------------
extern u8 DS18X20_FindSensor(u8 *diff, u8 *id);
extern u8 DS18X20_PowerStatus(u8 *id);
extern u8 DS18X20_StartMeasure(u8 *id);
extern u8 DS18X20_Read_Decicelsius(u8 *id, int16_t *decicelsius);
//*----------------------------------------------------------------------------
#endif
