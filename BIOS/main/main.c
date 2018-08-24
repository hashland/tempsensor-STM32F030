//*----------------------------------------------------------------------------
// main.c
//*----------------------------------------------------------------------------
#include "tools.h"
#include "timer.h"
#include "ds18x20.h"
#include "onewire.h"
#include "usb_main.h"
#include "usb_desc.h"
#include "usb_core.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
struct {
  u8 buf[64];
  T_TEMP *temp;
  u8 seq;
  u8 anz;
  u8 sensor;
  u8 SensorId[MAXSENSORS][OW_ROMCODE_SIZE];
} loc;  

struct timer main_timer;
//*----------------------------------------------------------------------------
// SYSTICK INT
//*----------------------------------------------------------------------------
void SysTick_Handler(void) 
{
  var.sys_ticks++;
} 
//*----------------------------------------------------------------------------
u32 clock_time(void) { return var.sys_ticks; }
//*----------------------------------------------------------------------------
// USB-STATE CHANGE
//*----------------------------------------------------------------------------
void USBX_StateChange(enum usb_state state)
{
  var.usb_state = state;
}
//*----------------------------------------------------------------------------
static u8 SearchSensors(void)
{
  u8 id[OW_ROMCODE_SIZE];
  u8 diff = OW_SEARCH_FIRST;
  
  loc.anz  = 0;
  while((diff != OW_LAST_DEVICE) && (loc.anz < MAXSENSORS)) {
    DS18X20_FindSensor(&diff, &id[0]);
    if(diff == OW_PRESENCE_ERR) { break; }
    if(diff == OW_DATA_ERR) { break; }
    MemCopy((u8 *)&loc.SensorId[loc.anz], (u8 *)&id, OW_ROMCODE_SIZE);

    dbg_dump((u8 *)&loc.SensorId[loc.anz], OW_ROMCODE_SIZE);  
      
    if(id[0] == DS18B20_FAMILY_CODE) { loc.anz++; }
  }
  return loc.anz;
}
//*----------------------------------------------------------------------------
// main() function
//*----------------------------------------------------------------------------
int main(void) 
{
  System_Init();
  Clock_Init();
  Port_Init();
  
  SysTick_Config(SystemCoreClock / 100);
  
#ifdef ERDEBUG
  dbg_init(115200);
  dbg("%r====> BIOS START <====%r");
#endif

  //==== USB STARTEN ====
  CRS->CR   = CRS_CR_AUTOTRIMEN | CRS_CR_CEN;     // Clock Recovery aktivieren
  USB_Init();
  
  //==== WAIT 500ms ====
  var.sys_ticks = 0;
  while(var.sys_ticks < 50);
  GPIOF->SETPORT = BIT(LED_R) | BIT(LED_G);

  //==== ONEWIRE INIT ====
  OW_Init();
  SearchSensors();
  loc.temp   = (T_TEMP *)&loc.buf;
  loc.sensor = 0;
  loc.seq    = 0;
  
  timer_set(&main_timer, 10);
  //==== MAIN LOOP ====
  while(true) {
    if(timer_expired(&main_timer)) {
      timer_reset(&main_timer);

      if(var.usb_state != USB_STATE_CONFIG) { continue; }

      //==== LED AN, MESSUNG STARTEN ====
      if(loc.seq == 0) {
        loc.temp->SensCnt = loc.anz;
        loc.temp->SensNr  = loc.sensor + 1;
        loc.temp->Power   = DS18X20_PowerStatus((u8 *)&loc.SensorId[loc.sensor]);
        MemCopy((u8 *)&loc.temp->SensId, (u8 *)&loc.SensorId[loc.sensor], OW_ROMCODE_SIZE);
        if(DS18X20_StartMeasure((u8 *)&loc.SensorId[loc.sensor]) == DS18X20_OK) {
          GPIOA->OTYPER &= ~BIT(ONEWIRE);
          GPIOF->CLRPORT = BIT(LED_R);
        }  
      }
      //==== LED AUS ====
      if(loc.seq == 1) {
        GPIOF->SETPORT = BIT(LED_R);
      }  
      //==== DATEN SENDEN ====
      if(loc.seq == 9) {
        GPIOA->OTYPER |= BIT(ONEWIRE);
        DS18X20_Read_Decicelsius((u8 *)&loc.SensorId[loc.sensor], (s16 *)&loc.temp->Temp);
        UserToPMABufferCopy(loc.buf, EP_HID_IN_TXADDR, 64);
        _SetEPTxCount(EP_HID_IN,  64);
        _SetEPTxStatus(EP_HID_IN, EP_TX_VALID);
      } 
      //==== SEQUENZ RESET ====
      if(++loc.seq == 10) { 
        if(++loc.sensor >= loc.anz) { loc.sensor = 0; }
        loc.seq = 0;
      }  
    }  
  }  
}
//*----------------------------------------------------------------------------
