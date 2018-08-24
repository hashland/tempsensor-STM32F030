//*----------------------------------------------------------------------------
// ERDEBUG.C
//*----------------------------------------------------------------------------
#include "erdebug.h"
#include "sys_config.h"
//*----------------------------------------------------------------------------
const char HX[] = "0123456789ABCDEF";
//*----------------------------------------------------------------------------
//  UART Send
//*----------------------------------------------------------------------------
static void dbg_send(u16 b)
{
  while(!(USART2->ISR & USART_ISR_TC));
  USART2->TDR = b;
}  

#define putchar(c) dbg_send(c)
//*----------------------------------------------------------------------------
// Debug - UART0 Init
//*----------------------------------------------------------------------------
void DBG_init(u32 baudrate)
{
  USART2->CR1 = USART_CR1_UE | USART_CR1_TE;
  USART2->CR2 = 0;
  USART2->CR3 = 0;
  USART2->BRR = SystemCoreClock / baudrate;
}
//*----------------------------------------------------------------------------
// HEX 4/8/16/32 BIT AUSGEBEN
//*----------------------------------------------------------------------------
static void dbg_hex(u32 val, u8 type)
{
  val <<= (32 - type);
  while(type) {
    dbg_send(HX[val >> 28]);
    val <<= 4;
    type -= 4;
  }
}
//*----------------------------------------------------------------------------
// DUMP MEMORY
//*----------------------------------------------------------------------------
void DBG_dump(u8 *adr, u32 anz)
{
  u32 blk = anz / 16;
  u32 ln, i;
  u8  c;

  if((blk * 16) != anz) { blk++; }

  for(ln = 0; ln < blk; ln++) {
    dbg_hex((u32)adr, 32);
    dbg_send(':');
    for(i = 0; i < 16; i++) {
      dbg_send(' ');
      if(ln * 16 + i < anz) { dbg_hex(*(adr + i), 8); }
                       else { dbg_send('.'); dbg_send('.');    }
    }
    dbg_send(' '); dbg_send(' ');
    for(i = 0; i < 16; i++) {
      if(ln * 16 + i < anz) {
        c = *(adr + i);
        if((c < 0x20) || (c > 0x7F)) { c = '.'; }
        dbg_send(c);
      }
    }
    adr += 16;
    dbg_send(13);
    dbg_send(10);
  }
}
//*----------------------------------------------------------------------------
// PRINT CHAR
//*----------------------------------------------------------------------------
static void printchar(char **str, int c)
{
  if(str) {
    **str = (char)c;
    ++(*str);
  } else {
    (void)putchar(c);           
  }
}
//*----------------------------------------------------------------------------
// PRINTS
//*----------------------------------------------------------------------------
static int prints(char **out, const char *string, int width, int pad)
{
	int pc = 0, padchar = ' ';

	if (width > 0) {
		int len = 0;
		const char *ptr;
		for(ptr = string; *ptr; ++ptr) { ++len; }
		if(len >= width) { width = 0; }
			           else { width -= len; }
		if(pad & PAD_ZERO) { padchar = '0'; }
	}
	if(!(pad & PAD_RIGHT)) {
		for(; width > 0; --width) {
			printchar(out, padchar);
			++pc;
		}
	}
	for(; *string ; ++string) {
		printchar(out, *string);
		++pc;
	}
	for(; width > 0; --width) {
		printchar(out, padchar);
		++pc;
	}
  return pc; 
}
//*----------------------------------------------------------------------------
// PRINT INT
//*----------------------------------------------------------------------------
static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[12];
	char *s;
	int  t, neg = 0, pc = 0;
	unsigned int u = (unsigned int)i;

	if(i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, pad);
	}

	if(sg && (b == 10) && (i < 0)) {
		neg = 1;
		u = (unsigned int)-i;
	}

	s = print_buf + 12 - 1;
	*s = '\0';

	while(u) {
		t = (unsigned int)u % b;
		if(t >= 10) {	t += letbase - '0' - 10; }
		*--s = (char)(t + '0');
		u /= b;
	}

	if(neg) {
		if(width && (pad & PAD_ZERO)) {
			printchar(out, '-');
			++pc;
			--width;
		} else {
			*--s = '-';
		}
	}
	return pc + prints(out, s, width, pad);
}
//*----------------------------------------------------------------------------
// PRINT
//*----------------------------------------------------------------------------
static int print(char **out, const char *format, va_list args)
{
  int width, pad, pc = 0;
  char scr[2], *s;
  
  for(; *format != 0; ++format) {
    if(*format == '%') {
      ++format;
      width = 0;
      pad   = 0;
      if(*format == 0) { break; }
      if(*format == '%') { goto out; }
      if(*format == '-') {
        ++format;
        pad = PAD_RIGHT; 
      }  
      if(*format == '0') {
        ++format;
        pad |= PAD_ZERO; 
      }
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			switch(*format) {
			  //............................................................
			  case 's':
  				s = (char *)va_arg(args, int);
	  			pc += prints(out, s ? s : "(null)", width, pad);
		  		continue;
			  //............................................................
			  case 'd':
  				pc += printi(out, va_arg(args, int), 10, 1, width, pad, 'a');
	  			continue;
			  //............................................................
			  case 'u':
  				pc += printi(out, va_arg(args, int), 10, 0, width, pad, 'a');
	  			continue;
			  //............................................................
			  case 'x':
  				pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'A');
	  			continue;
			  //............................................................
			  case 'b':
  				pc += printi(out, va_arg(args, int),  2, 0, width, pad, 'a');
	  			continue;
			  //............................................................
			  case 'c':
  				scr[0] = (char)va_arg(args, int);
	  			scr[1] = '\0';
		  		pc += prints(out, scr, width, pad);
			  	continue;
			  //............................................................
			  case 'r':
          pc += prints(out, "\r\n", 0, 0);
  				continue;
			  //............................................................
			  case 'B':
  			  if(va_arg(args, int)) { pc += prints(out, "TRUE",  width, 0); }
	  		                   else { pc += prints(out, "FALSE", width, 0); }
		  		continue;
			  //............................................................
		  } 		
    } else {
out:      
      printchar(out, *format);
      ++pc;
    }      
  }
  if(out) { **out = 0; }
  va_end(args);
  return pc;
}
//*----------------------------------------------------------------------------
// PRINTF
//*----------------------------------------------------------------------------
int printf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  return print(0, format, args);  
}
//*----------------------------------------------------------------------------
// SPRINTF
//*----------------------------------------------------------------------------
int sprintf(char *out, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  return print(&out, format, args);  
}
//*----------------------------------------------------------------------------
