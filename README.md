# artpi_gcc.stldr
artpi_cube.stldr qspi-flash algorithm created by arm-none-eabi-gcc on ubuntu

# linux下用stm32cubeIDE，或者直接用gcc编译器，生成ART-PI的qspi flash的烧录文件（stldr文件）
# D1 参考工程
ubuntu下，git clone如下工程。这个是MDK下的工程，可以直接生成artpi的stldr算法，经过我验证，确实可以用。
我们的目的是，将其转化为stm32cubeIDE的工程，用arm-none-eabi-gcc来编译和生成stldr算法。
```
git clone https://gitee.com/wangzc-git/artpi-cube-stldr.git
```

# D2 修改点
因为整个工程在MDK下是可以运行的，因为代码是没有问题的，主要任务是针对gcc编译器来调整。尤其是涉及ld文件的sections的定义，以及部分代码和函数的ld处理。

## Dev_Inf.c
这个结构体数据添加ld属性，给它分配一个单独的section
![输入图片说明](https://foruda.gitee.com/images/1729349954999357443/f432de04_8231215.png "屏幕截图")

## Loader_Src.c
这4个核心函数添加ld属性，主要是让它Keep不被优化掉。
![输入图片说明](https://foruda.gitee.com/images/1729350039144977003/69ad0580_8231215.png "屏幕截图")
![输入图片说明](https://foruda.gitee.com/images/1729350060730300550/7e2886f4_8231215.png "屏幕截图")
![输入图片说明](https://foruda.gitee.com/images/1729350073834029181/c428405e_8231215.png "屏幕截图")
![输入图片说明](https://foruda.gitee.com/images/1729350090329483169/68de40b0_8231215.png "屏幕截图")

## 在project根目录下添加链接文件
/home/xujc/STM32CubeIDE/workspace_1.15.0/artpi-cube-stldr/STM32_FLASH.ld
```ld
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
  RAM (xrw)       : ORIGIN = 0x24000004, LENGTH = 0xFE00-4
}

/* Define output sections */
SECTIONS
{
  /* The program code and other data goes into FLASH */
  .text :
  {
    . = 0x00;
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    _etext = .;        /* define a global symbols at end of code */
  } >RAM :Loader

  /* Constant data goes into FLASH */
  .rodata :
  {
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
  } >RAM :Loader

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >RAM
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >RAM :Loader

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >RAM :Loader
  
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >RAM :Loader
  
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >RAM :Loader

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    _edata = .;        /* define a global symbol at data end */
  } >RAM :Loader

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

  /* User_heap_stack section, used to check that there is enough "RAM" Ram type memory left */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM :Loader

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }

  .storage_info :
  {
    . = ALIGN(8);
    KEEP ( *Dev_Inf.o (.storage_info .storage_info*) )
  } >RAM :SgInfo
}
```

说明，这个文件的核心就是用PHDRS定义了2个segment（Loader和SgInfo），没有这个就不行。

## 在project根目录下添加Makefile文件，用于编译和一些烧录操作
/home/xujc/STM32CubeIDE/workspace_1.15.0/artpi-cube-stldr/Makefile
```makefile
# Toolchain configuration
CC := arm-none-eabi-gcc
SIZE := arm-none-eabi-size
OBJDUMP := arm-none-eabi-objdump

# Project settings
TARGET := artpi_cube_stldr.stldr
MAPFILE := $(TARGET:.stldr=.map)

# Source files
SRCS := $(wildcard ./Drivers/*.c) \
				$(wildcard ./Template/*.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_gpio.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_rcc.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_rcc_ex.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_pwr.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_pwr_ex.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_flash.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_cortex.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_exti.c) \
				$(wildcard ./Library/HALLIB/Src/stm32h7xx_hal_qspi.c) \

# Object files
OBJS := $(SRCS:%.c=%.o)

# Include directories
INC_DIRS := -I ./Template \
						-I ./Library/CORE \
            -I ./Library/HALLIB/Inc/Legacy \
            -I ./Library/HALLIB/Inc \
						-I ./Drivers \
            

# Macro definitions
DEFINES := -DDEBUG -DUSE_HAL_DRIVER -DSTM32H750xx

# Compilation options
CFLAGS := -mcpu=cortex-m7 -std=gnu11 -g $(DEFINES) $(INC_DIRS) \
          -O0 -Wall -fstack-usage -ffunction-sections -fdata-sections \
          -MMD -MP --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb

# Linker options
#LDFLAGS := -mcpu=cortex-m7 -T"STM32H750XBHX_EXFLASH.ld" --specs=nosys.specs \
#           -Wl,-Map=$(MAPFILE) -Wl,--gc-sections -static --specs=nano.specs \
#           -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group

LDFLAGS := -mcpu=cortex-m7 -T"STM32_FLASH.ld" --specs=nosys.specs \
           -Wl,-Map=$(MAPFILE) -Wl,--gc-sections -static --specs=nano.specs \
           -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group

# Default target
all: $(TARGET)

# Linking target
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)
	$(SIZE) $(TARGET)

# Dependency files
-include $(OBJS:.o=.d)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET) $(MAPFILE)
	find . -name "*.o"|xargs rm -rf
	find . -name "*.d"|xargs rm -rf
	find . -name "*.su"|xargs rm -rf

# List target
list: $(TARGET)
	$(OBJDUMP) -h -S $(TARGET) > $(TARGET:.stldr=.list)

.PHONY: all clean list




ext_load_path=/home/xujc/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/ExternalLoader
stprog_path=/home/xujc/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin

1:
	readelf -a  $(TARGET) > log; \
	head -100 log; \
	cp $(TARGET) $(ext_load_path); \
	ls -la $(ext_load_path)/$(TARGET)
		
2:
	$(stprog_path)/STM32_Programmer_CLI -c port=SWD freq=4000 -r32 0x08000000 100

3:
	$(stprog_path)/STM32_Programmer_CLI -c port=SWD freq=4000 -el $(TARGET) -r32 0x90000000 100

```

这个Makefile的用法：  
make：编译生成stdlr文件  
make clean：清理工程  
make 1：用readelf -a命令分析stdlr文件的结构，并将这个stdlr文件拷贝到stm32CubeProgrammer的ExternalLoader路径下去，这样stm32CubeProgrammer图形化界面可以使用  
make 2：命令行模式读内部flash内容  
make 3：命令行模式读qspi-flash内容，使用我们编译好的stdlr文件  

# D3 运行结果
执行make编译
![输入图片说明](https://foruda.gitee.com/images/1729351206291576583/e3257172_8231215.png "屏幕截图")

执行make 3用命令行模式使用stldr文件读取qspi-flash
![输入图片说明](https://foruda.gitee.com/images/1729351487819868999/503c7774_8231215.png "屏幕截图")

执行make 1将stldr文件更新到ExternalLoader目录下去，并用readelf查看stldr文件的格式
![输入图片说明](https://foruda.gitee.com/images/1729351583012721992/0b98d18b_8231215.png "屏幕截图")
![输入图片说明](https://foruda.gitee.com/images/1729351814119160612/237efeed_8231215.png "屏幕截图")

打开stm32cubeProgrammer，EL界面可以看到正确识别我们生成的artpi_cube_stldr，勾选
![输入图片说明](https://foruda.gitee.com/images/1729351896538945770/63f67789_8231215.png "屏幕截图")

stm32cubeProgrammer的烧录界面，我们烧录一个图片的bin文件进去，可以看到烧录成功，校验成功
![输入图片说明](https://foruda.gitee.com/images/1729352014866967564/f50fdc42_8231215.png "屏幕截图")

stm32cubeProgrammer的读界面，读出0x90000000地址的内容，然后与我们直接用hex编辑器打开bin文件，进行对比，可见内容完全一样。
![输入图片说明](https://foruda.gitee.com/images/1729352091081570884/56243486_8231215.png "屏幕截图")
![输入图片说明](https://foruda.gitee.com/images/1729352163133960255/9c8f432e_8231215.png "屏幕截图")

# D4 总结与反思
在MDK下很轻松就可以制作一个stldr文件，但是用stm32cubeIDE却折腾了很久。现在反思和复盘整个过程，有几点值得注意的：  
1）换ram区域，是否可以？----DTCM不可以，AXISRAM可以。  
2）stm32h7xx_hal_qspi.c中的QSPI_WaitFlagStateUntilTimeout函数部分，使用了HAL_GetTick函数，是否需要注释掉？  ----不影响  
3）Load_Src.c注释掉read函数，是否可以？----不影响  
4）Load_Src.c中那4个函数，不使用used属性----不行，函数会被优化掉  
5) ld文件的PHDRS（program headers）----不可少，否则stm32cubeProgrammer直接不识别stldr文件。  

由于gcc下ld文件比较复杂，因此建议先在MDK下制作，确保整个工程可以成功产生stldr文件，然后考虑移植到stm32cubeIDE环境下去。  
