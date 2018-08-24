/*----------------------------------------------------------------------------*/
/* STM32F042 (32k FLASH, 6k RAM, 48MHz)
/*----------------------------------------------------------------------------*/
MEMORY
{
  /* Define each memory region */
  FlashBoot  (rx) : ORIGIN = 0x08000000, LENGTH = 0x00002000    /*   8k */
  FlashBios  (rx) : ORIGIN = 0x08002000, LENGTH = 0x00005C00    /*  23k */
  Eeprom     (rx) : ORIGIN = 0x08007C00, LENGTH = 0x00000400    /*   1k */
  RamBoot   (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00000400    /*   1k */
  RamBios   (rwx) : ORIGIN = 0x20000400, LENGTH = 0x00001400    /*   5k */
  
}
/*----------------------------------------------------------------------------*/
/* Define a symbol for the top of each memory region */
__top_FlashBoot = 0x08002000;
__top_FlashBios = 0x08008000;
__top_RamBoot   = 0x20000400;
__top_RamBios   = 0x20001800;
/*----------------------------------------------------------------------------*/
ENTRY(ResetISR)

SECTIONS
{
	/*==== MAIN TEXT SECTION ====*/	
	.text : ALIGN(4)
	{
		FILL(0xff)
		KEEP(*(.isr_vector))
		. = 0x00000D0; 
		KEEP(*(.bootinfo*))
   . = 0x00000100;
		KEEP(*(.bootjump .bootjump.*))
   . = 0x00000120;
		*(.after_vectors*)
		*(.text*)
		*(.rodata .rodata.*)
		. = ALIGN(4);
	} > FlashBoot

	_etext = .;
		
	/*==== RAM-VECTORS ====*/
  .vect : ALIGN(4)
  {
    PROVIDE(_ram_vectors = .);
    . = 0x00000100;
  } > RamBoot
		
	/*==== MAIN DATA SECTION ====*/
	.data : ALIGN(4)
	{
		FILL(0xFF)
		_data = .;
		*(.data*)
		. = ALIGN(4) ;
		_edata = .;
	} > RamBoot AT > FlashBoot

	/*==== MAIN BSS SECTION ====*/
	.bss : ALIGN(4)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4) ;
		_ebss = .;
		PROVIDE(end = .);
	} > RamBoot

	/*==== BIOSDATA ====*/	
  .biosdata : ALIGN(4)
	{
    . = 0x00000100;
		KEEP(*(.biosinfo .biosinfo.*))
    . = 0x00000120;
		KEEP(*(.biosjump .biosjump.*))
	} > FlashBios

	/*==== EEPROM ====*/	
  .eep : ALIGN(4)
	{
	  *(.eeprom*)
	} > Eeprom


	PROVIDE(_vStackTop = __top_RamBios);
	PROVIDE(_filesize  = _etext + SIZEOF(.data));
}
/*================================================================*/
