//*----------------------------------------------------------------------------
// bootfunc.c
//*----------------------------------------------------------------------------
#include "sys_config.h"
#include "usb_cdc.h"
#include "tools.h"
#include "bootfunc.h"
#include "erdebug.h"
//*----------------------------------------------------------------------------
// VARIABLEN
//*----------------------------------------------------------------------------
struct {
//  ATTR_ALIGNED(4) u8 PktBuf[256];
  enum   msg_state  state;  // Message State
  struct msg_struct msg;    // Message Struct
  u8  chksum;               // Checksumme
  u16 out_pos;              // Position in Out-Buffer    
  u16 crypt[8];             // Crypt-Bytes auf CPU-Seriennummer
} bf; 
//*----------------------------------------------------------------------------
// SENDE PAKET
//*----------------------------------------------------------------------------
static void Send_Packet(u8 status, u16 cnt)
{
  u8 *ptr = (u8 *)&bf.msg;
  bf.msg.com_status = status;
  bf.msg.byte_cnt   = cnt;
  bf.chksum         = 0;
  for(int i = 0; i < (cnt + 6); i++) { bf.chksum ^= ptr[i]; }
  bf.msg.data.B[cnt] = bf.chksum;
  CDC_Tx(ptr, cnt + 7);
}
//*----------------------------------------------------------------------------
// BOOT ACTION
//*----------------------------------------------------------------------------
static void Boot_Action(void)
{
  ATTR_ALIGNED(4) u8  buf[256];
  u8  rc   = STATE_NOCOM;
  u16 rcnt = 0;
  u32 adr;

//  dbg("COM: %02x%r", bf.msg.com_status);

  boot.boot_watch = 0;      // Bootloader-Watchdog Reset
  switch(bf.msg.com_status) {
    //......................................................
    // F1 - RESET
    //......................................................
    case BOOT_SET_RESET:
      if(bf.msg.data.B[0] != 'R') { rc = STATE_VALUE; break; }
      boot.main_state = MAIN_SCAN_BIOS;  
      rc   = STATE_OK;
      break;
    //......................................................
    // F2 - BOOTINFO LESEN
    //......................................................
    case BOOT_GET_BOOTINFO:
      rcnt = sizeof(T_BOOTINFO);
      rc   = STATE_OK;
      MemCopy((u8 *)&bf.msg.data, (u8 *)&bootinfo, sizeof(T_BOOTINFO));
      break;
    //......................................................
    // F3 - BIOSINFO LESEN
    //......................................................
    case BOOT_GET_BIOSINFO:
      rcnt = sizeof(T_BIOSINFO);
      rc   = STATE_OK;
      MemCopy((u8 *)&bf.msg.data, (u8 *)&biosinfo, sizeof(T_BIOSINFO));
      break;
    //......................................................
    // F5 - GET CPUINFO
    //......................................................
    case BOOT_GET_CPUINFO:
      MemCopy((u8 *)&bf.msg.data.B[0], (u8 *)UNIQUE_ID, 12);
      bf.msg.data.L[3] = bf.msg.data.L[0] ^ bf.msg.data.L[1];
      bf.msg.data.L[4] = bf.msg.data.L[1] ^ bf.msg.data.L[2];
      bf.msg.data.L[5] = bf.msg.data.L[2] ^ bf.msg.data.L[0];

      for(int i = 0; i < 4; i++) {
        MemCopy((u8 *)&buf, (u8 *)&bf.msg.data.L[i], 12);
        u32 tmp = Crc32((u32)&buf, 12);
        bf.crypt[i * 2 + 0] = tmp >> 16;
        bf.crypt[i * 2 + 1] = tmp;
      }  
//      dbg_dump((u8 *)crypt, 16);
      rcnt = 12;
      rc   = STATE_OK;
      break;   
    //......................................................
    // F6 - READ MEM
    // L[0] = ADR, W[2] = CRYPT[MEM], B[6] = MEM
    //......................................................
    case BOOT_READ_MEM:
      if(bf.msg.byte_cnt != 7) { rc = STATE_COUNT; break; }
      if(bf.msg.data.W[2] != bf.crypt[bf.msg.data.B[6] & 0x07]) { 
        rc = STATE_CRYPT; 
        break;
      }
      adr = bf.msg.data.L[0];
      switch(bf.msg.data.B[6]) {
        //++++++++++++++++++++++++++++++++++++++++++++++++
        case MEMORY_FLASH:    // 0
          if((adr < BIOSSTART) || (adr > (FLASHSTART + FLASHSIZE - 256))) {
            rc = STATE_VALUE;
            goto finish;
          }
          goto docopy;
        //++++++++++++++++++++++++++++++++++++++++++++++++
        case MEMORY_RAM:      // 1
          if((adr < RAMSTART) || (adr > (RAMSTART + RAMSIZE - 256))) {
            rc = STATE_VALUE;
            goto finish;
          } 
          goto docopy;
        //++++++++++++++++++++++++++++++++++++++++++++++++
docopy:   
          MemCopy((u8 *)&bf.msg.data, (u8 *)adr, 256);
//dosend:   
          rcnt = 256;
          rc   = STATE_OK;
          goto finish;  
      }
      rc = STATE_UNKNOWN;
      break; 
    //......................................................
    // F9 - WRITE FLASH 
    // L[0] = ADR, W[2] = CRYPT[6], B[6]...B[261]
    //......................................................
    case BOOT_WRITE_FLASH:
      if(!boot.is_boot) { rc = STATE_NOBOOT; break; }
      if(bf.msg.byte_cnt != 262) { rc = STATE_COUNT; break; }
      if(bf.msg.data.W[2] != bf.crypt[6]) { rc = STATE_CRYPT; break; }

      if((bf.msg.data.L[0] < BIOSSTART) || (bf.msg.data.L[0] > (FLASHEND - 256))) {
        rc = STATE_VALUE; break;
      } 

      MemCopy((u8 *)&buf, (u8 *)&bf.msg.data.B[6], 256);
      //=== DECRYPT AB 2. PAKET ===
      if(bf.msg.data.L[0] != BIOSSTART) {
        for(adr = 0; adr < 256; adr++) {
          u8 b = buf[adr];
          b ^= *(u8 *)(BIOSSTART + 0xE0 + (adr & 0x1F));
          b += ((adr & 0x3F) - 0xAB);
          b ^= *(u8 *)(BIOSSTART + 0xD0 + (adr & 0x1F));
          b -= ((adr & 0x1F) + 0x65);
          b ^= *(u8 *)(BIOSSTART + 0xC0 + (adr & 0x1F));
          buf[adr] = b;
        }  
      }  
      //===============
      rc = FLASH_Write(bf.msg.data.L[0], (u32 *)&buf, 256 / 4);
      if(rc == FLASH_COMPLETE) { rc = STATE_OK; }
                          else { rc += 0xD0;    }
      break;     
    //......................................................
  }
finish:  
  Send_Packet(rc, rcnt);
}
//*----------------------------------------------------------------------------
// BOOTFUNC
//*----------------------------------------------------------------------------
void BootFunc_Data(u8 *ptr, u8 cnt)
{
//  dbg("BOOTFUNC (%d)%r", cnt);  
//  dbg_dump(ptr, cnt);
  
  for(int i = 0; i < cnt; i++) {
    u8 b = ptr[i];
    switch(bf.state) {
      //....................................................
      case MSG_START:
        if(b == MESSAGE_START) {
          bf.msg.start_byte = b;        // Für Answer merken
          bf.chksum = 0;                // Checksumme = 0
          bf.state  = MSG_SEQ_NUM;      // Jetzt auf Sequence-Num warten
        }  
        break;
      //....................................................
      case MSG_SEQ_NUM:
        bf.msg.seq_num = b;             // Sequence-Num speichern
        bf.state       = MSG_SIZE_LO;   // Jetzt auf Size LO warten
        break;
      //....................................................
      case MSG_SIZE_LO:
        bf.msg.byte_cnt = b;            // Size-LO speichern
        bf.state     = MSG_SIZE_HI;     // Jetzt auf Size HI warten
        break;
      //....................................................
      case MSG_SIZE_HI:
        bf.msg.byte_cnt |= (b << 8);    // Size-HI speichern
        if(bf.msg.byte_cnt > (BUF_SIZE - 1)) { bf.state = MSG_START; }   // Von vorn beginnen
                                        else { bf.state = MSG_COMMAND; } // Jetzt auf Command warten
        break;
      //....................................................
      case MSG_COMMAND:
        bf.msg.com_status = b;           // Command speichern
        bf.state = MSG_TOKEN;
        break;
      //....................................................
      case MSG_TOKEN:
        bf.msg.token_byte = b;           // Für Answer merken
        if(b == TOKEN) { 
          if(bf.msg.byte_cnt) { bf.state = MSG_DATA;  }    // Jetzt Daten empfangen
                         else { bf.state = MSG_CHECK; }    // Oder nur Checksumme
        } else { bf.state = MSG_START; }  // Von vorn beginnen
        bf.out_pos = 0;
        break;
      //....................................................
      case MSG_DATA:
        bf.msg.data.B[bf.out_pos++] = b;    // Daten speichern
        if(bf.out_pos == bf.msg.byte_cnt) { bf.state = MSG_CHECK; }
        break;
      //....................................................
      case MSG_CHECK:
        if(b != bf.chksum) { 
          bf.state = MSG_START;       // Checksum Error, von vorn beginnen
          break;
        } else { 
          bf.state = MSG_EXEC;        // Befehl ausführen
        }  
      //....................................................
      case MSG_EXEC:
        Boot_Action();
        bf.state = MSG_START;
        break;
      //....................................................
    }
    bf.chksum ^= b;
  }
}
//*----------------------------------------------------------------------------
// BOOTFUNC INIT
//*----------------------------------------------------------------------------
void BootFunc_Init(void)
{
  bf.state = MSG_START;
  boot.boot_watch = 0;         // Bootloader-Watchdog Reset
  MemSet((u8 *)&bf.crypt, 0xAB, sizeof(bf.crypt));
}
//*----------------------------------------------------------------------------
