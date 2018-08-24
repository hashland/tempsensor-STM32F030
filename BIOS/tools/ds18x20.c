//*----------------------------------------------------------------------------
// ds18x20.c
//*----------------------------------------------------------------------------
#include "ds18x20.h"
#include "onewire.h"
#include "tools.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
#define CRC8INIT    0x00
#define CRC8POLY    0x18              // 0X18 = X^8+X^5+X^4+X^0
//*----------------------------------------------------------------------------
static u8 crc8(u8 *data, u16 anz)
{
	u8  bit_counter, b, feedback_bit;
	u8 crc = CRC8INIT;
	for(int loop_count = 0; loop_count != anz; loop_count++) {
		b = data[loop_count];
		bit_counter = 8;
		do {
			feedback_bit = (crc ^ b) & 0x01;
			if(feedback_bit == 0x01) {	crc = crc ^ CRC8POLY;	}
			crc = (crc >> 1) & 0x7F;
			if(feedback_bit == 0x01) {	crc = crc | 0x80;			}
			b = b >> 1;
			bit_counter--;
		} while (bit_counter > 0);
	}
	return crc;
}
//*----------------------------------------------------------------------------
static u8 read_scratchpad(u8 *id, u8 *sp, u8 n)
{
	uint8_t ret;

	OW_Command(DS18X20_READ, id);
	for(int i = 0; i < n; i++) { sp[i] = OW_ReadByte(); }
	if(crc8(&sp[0], DS18X20_SP_SIZE)) {
		ret = DS18X20_ERROR_CRC;
	} else {
		ret = DS18X20_OK;
	}
	return ret;
}
//*----------------------------------------------------------------------------
// convert scratchpad data to physical value in unit decicelsius
//*----------------------------------------------------------------------------
static s16 raw_to_decicelsius(u8 familycode, u8 *sp)
{
  
	uint16_t measure;
	uint8_t  negative;
	int16_t  decicelsius;
	uint16_t fract;

//  dbg_dump((u8 *)sp, 8);

	measure = sp[0] | (sp[1] << 8);
	//measure = 0xFF5E; // test -10.125
	//measure = 0xFE6F; // test -25.0625
	
	if(familycode == DS18S20_FAMILY_CODE) {   // 9 -> 12 bit if 18S20
		/* Extended measurements for DS18S20 contributed by Carsten Foss */
		measure &= (uint16_t)0xfffe;   // Discard LSB, needed for later extended precicion calc
		measure <<= 3;                 // Convert to 12-bit, now degrees are in 1/16 degrees units
		measure += (16 - sp[6]) - 4;   // Add the compensation and remember to subtract 0.25 degree (4/16)
	}

	// check for negative 
	if(measure & 0x8000) {
		negative = 1;       // mark negative
		measure ^= 0xffff;  // convert to positive => (twos complement)++
		measure++;
	} else {
		negative = 0;
	}

	//=== clear undefined bits for DS18B20 != 12bit resolution ===
	if((familycode == DS18B20_FAMILY_CODE) || (familycode == DS1822_FAMILY_CODE)) {
		switch(sp[DS18B20_CONF_REG] & DS18B20_RES_MASK) {
		  case DS18B20_9_BIT:
			  measure &= ~(DS18B20_9_BIT_UNDF);
			  break;
		  case DS18B20_10_BIT:
			  measure &= ~(DS18B20_10_BIT_UNDF);
			  break;
		  case DS18B20_11_BIT:
  			measure &= ~(DS18B20_11_BIT_UNDF);
	  		break;
		  default:
			  break;
		}
	}

	decicelsius = (measure >> 4);
	decicelsius *= 10;

	// decicelsius += ((measure & 0x000F) * 640 + 512) / 1024;
	// 625/1000 = 640/1024
	fract = (measure & 0x000F) * 640;
	if(!negative) { fract += 512;	}
	fract /= 1024;
	decicelsius += fract;

	if(negative) { decicelsius = -decicelsius; }

//	dbg("MEASURE: %d%r", decicelsius);

	if ( /* decicelsius == 850 || */ decicelsius < -550 || decicelsius > 1250 ) {
		return DS18X20_INVALID_DECICELSIUS;
	} else {
		return decicelsius;
	}
	
	return 0;
}

//*----------------------------------------------------------------------------
// find DS18X20 Sensors on 1-Wire-Bus
// input/ouput: diff is the result of the last rom-search
//              *diff = OW_SEARCH_FIRST for first call
//   output: id is the rom-code of the sensor found */
//*----------------------------------------------------------------------------
u8 DS18X20_FindSensor(u8 *diff, u8 *id)
{
	u8 ret = DS18X20_OK;
	while(true) {
		*diff = OW_RomSearch(*diff, &id[0]);
		if((*diff == OW_PRESENCE_ERR) || 
		   (*diff == OW_DATA_ERR)     ||
		   (*diff == OW_LAST_DEVICE)) { 
			ret = DS18X20_ERROR;
			break;
		} else {
			if((id[0] == DS18B20_FAMILY_CODE) || 
			   (id[0] == DS18S20_FAMILY_CODE) ||
			   (id[0] == DS1822_FAMILY_CODE)) { 
				break;
			}
		}
	};
	return ret;
}
//*----------------------------------------------------------------------------
// get power status of DS18x20 
//   input:   id = rom_code 
//   returns: DS18X20_POWER_EXTERN or DS18X20_POWER_PARASITE */
//*----------------------------------------------------------------------------
u8 DS18X20_PowerStatus(u8 *id)
{
	uint8_t pstat;

	OW_Command(DS18X20_READ_POWER_SUPPLY, id);
	pstat = OW_IoBit(1);
	OW_Reset();
	return(pstat) ? DS18X20_POWER_EXTERN : DS18X20_POWER_PARASITE;
}
//*----------------------------------------------------------------------------
// start measurement (CONVERT_T) for all sensors if input id==NULL 
// or for single sensor where id is the rom-code
//*----------------------------------------------------------------------------
u8 DS18X20_StartMeasure(u8 *id)
{
  u8 ret = DS18X20_START_FAIL;  
  
  OW_Reset();
  if(OW_PinState()) {
    OW_Command(DS18X20_CONVERT_T, id);    
    ret = DS18X20_OK;
  }
  return ret;
}
//*----------------------------------------------------------------------------
// reads temperature (scratchpad) of sensor with rom-code id
//   output: decicelsius 
//   returns DS18X20_OK on success */
//*----------------------------------------------------------------------------
u8 DS18X20_Read_Decicelsius(u8 *id, int16_t *decicelsius)
{
	u8 sp[DS18X20_SP_SIZE];
	u8 ret;
	
	OW_Reset();
	ret = read_scratchpad(id, sp, DS18X20_SP_SIZE);
	if(ret == DS18X20_OK) {
		*decicelsius = raw_to_decicelsius(id[0], sp);
	}
	return ret;
}
//*----------------------------------------------------------------------------
