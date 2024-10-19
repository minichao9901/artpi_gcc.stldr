/*
*****************************************************************************
**
**  File        : stm32_flash.ld
**
**  Abstract    : Linker script for STM32F103ZG Device with
**                1024KByte FLASH, 96KByte RAM
**
**  Target      : STMicroelectronics STM32
**
**  Environment : Atollic TrueSTUDIO(R)
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Init)

/* Highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);	/* end of "RAM" Ram type memory */

_Min_Heap_Size = 0x1000;	/* required amount of heap  */
_Min_Stack_Size = 0x1000;	/* required amount of stack */

/* Generate 2 segment for loader code and device info */
PHDRS {Loader PT_LOAD ; SgInfo PT_LOAD ; }

/* Specify the memory areas */
MEMORY
{
  ROM (xr)       : ORIGIN = 0x0000000, LENGTH = 32K
  RAM (xrw)       : ORIGIN = 0x20000004, LENGTH = 128K-4
}

/* Define output sections */
SECTIONS
{

  .storage_info :
  {
    . = ALIGN(8);
    KEEP ( *Dev_Inf.o (.storage_info .storage_info*) )
  } >ROM :SgInfo

  /* The program code and other data goes into FLASH */
    .prg_func :
    {
    	  . = ALIGN(8);
     KEEP(*Load_Src.o(.prg_func .prg_func*) )
    } >ROM :Loader

  .text :
  {
    	  . = ALIGN(8);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    _etext = .;        /* define a global symbols at end of code */
  } >ROM :Loader 

  /* Constant data goes into FLASH */
  .rodata :
  {
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
  } >ROM :Loader 

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >RAM :Loader
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >ROM :Loader 

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >ROM :Loader 
  
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ROM :Loader 
  
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >ROM :Loader 

  /* Initialized data sections goes into RAM :Loader, load LMA copy after code */
  .data : 
  {
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    _edata = .;        /* define a global symbol at data end */
  } >ROM :Loader 

  /* Uninitialized data section */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    *(.bss)
    *(.bss*)
    *(COMMON)
    _ebss = .;         /* define a global symbol at bss end */
  } >RAM :Loader 

  /* User_heap_stack section, used to check that there is enough "RAM :Loader" Ram type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM :Loader 





}
