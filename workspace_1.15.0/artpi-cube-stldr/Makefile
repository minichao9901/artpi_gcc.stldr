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
