#  __________________
# < Matrix4 makefile >
#  ------------------
#        \   ^__^
#         \  (oo)\_______
#            (__)\       )\/\
#                ||----w |
#                ||     ||
#

#TODO firmware version incrementing

TARGET                 = MUEB

TOOLCHAIN             := arm-none-eabi
CC                    := $(TOOLCHAIN)-gcc
CXX                   := $(TOOLCHAIN)-g++
AS                    := $(TOOLCHAIN)-as
OBJCOPY	              := $(TOOLCHAIN)-objcopy
SIZE 		      := $(TOOLCHAIN)-size
C_STANDARD	      := -std=gnu11
CXX_STANDARD 	      := -std=gnu++11

DEFS                  := -DSTM32F030x8 -DUSE_HAL_DRIVER  -DUSE_FULL_LL_DRIVER

COMMON_COMPILER_FLAGS := -Os -g -fstack-usage -Wall -Wextra -Wpacked -Winline  -mcpu=cortex-m0 -mthumb
C_FLAGS               := $(COMMON_COMPILER_FLAGS) $(C_STANDARD)
CXX_FLAGS             := $(COMMON_COMPILER_FLAGS) $(CXX_STANDARD) -fno-rtti -fno-exceptions -fno-threadsafe-statics 

LD_FLAGS              := -specs=nosys.specs -specs=nano.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=build/$(TARGET).map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80 -Wl,--start-group -lc -lm -Wl,--end-group

#TODO script this
INCLUDES               = -I Inc
INCLUDES              += -isystem Drivers/CMSIS/Device/ST/STM32F0xx/Include -isystem Drivers/CMSIS/Include -isystem Drivers/STM32F0xx_HAL_Driver/Inc
INCLUDES              += -isystem Drivers/ioLibrary_Driver/Internet/DHCP -isystem Drivers/ioLibrary_Driver/Ethernet -isystem Drivers/ioLibrary_Driver/Ethernet/W5500


C_FILES               := mac_eeprom.c dhcp_buffer.c stm32f0xx_it.c system_stm32f0xx.c main.c
ASM_FILES             := startup_stm32f030x8.s
CPP_FILES             := internal_anim.cpp firm_update.cpp main2.cpp network.cpp window.cpp stm32_flash.cpp

ELF                    = build/$(TARGET)_fw.elf
ELF_FW_UPDATE          = build/$(TARGET)_fwupdate.elf

HEX_FW                 = build/$(TARGET)_fw.ihex
HEX_FW_UPDATE          = build/$(TARGET)_fwupdate.ihex
HEX                    = build/$(TARGET).ihex

LDSCRIPT               = STM32F030C8_FLASH.ld

####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####

C_OBJS                 = $(addprefix build/c_,$(C_FILES:.c=.o))
CPP_OBJS               = $(addprefix build/cpp_,$(CPP_FILES:.cpp=.o))
ASM_OBJS               = $(addprefix build/asm_,$(ASM_FILES:.s=.o))


all: $(HEX) | build_dir

.PHONY: build_dir clean src

build_dir: 
	@mkdir -p build

build/c_%.o: Src/%.c | build_dir
	@echo "[CC]	$(notdir $<)"
	$(CC) $(C_FLAGS) $(DEFS) $(INCLUDES) -c -o $@ $<

build/asm_%.o: startup/%.s | build_dir
	@echo "[CC]	$(notdir $<)"
	$(CC) $(C_FLAGS) -c -o $@ $<

build/cpp_%.o: Src/%.cpp | build_dir
	@echo "[CXX]	$(notdir $<)"
	$(CXX) $(CXX_FLAGS) $(DEFS) $(INCLUDES) -c -o $@ $<

clean:
	@echo "[RM]     build/*"
	@rm -f build/*

src: $(C_OBJS) $(CPP_OBJS) $(ASM_OBJS) | build_dir

$(ELF): $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS)
	@echo "[LD]     $@"
	$(CXX) -o $@ $(CXX_FLAGS) $(LD_FLAGS) -T$(LDSCRIPT) $? Drivers/wiznet_driver/ioLibrary.a Drivers/STM32F0xx_HAL_Driver/hal.a 
	$(SIZE) $@

$(ELF_FW_UPDATE): build/cpp_refurbish.o build/cpp_stm32_flash.o
	@echo "[LD]     $@"
	$(CXX) -o $@ -Os -g -fstack-usage -Wall -Wextra -Wpacked -Winline  -mcpu=cortex-m0 -mthumb -std=gnu++11 -fno-rtti -fno-exceptions -fno-threadsafe-statics   -specs=nano.specs -static -Wl,--gc-sections -nostartfiles -nostdlib -nodefaultlibs  -Trefurbish_linker.ld build/cpp_refurbish.o  build/cpp_stm32_flash.o -Wl,--strip-all


$(HEX_FW): $(ELF)
	@echo "[OBJCOPY]$< $@"
	arm-none-eabi-objcopy -O ihex $< $@

$(HEX_FW_UPDATE): $(ELF_FW_UPDATE)
	@echo "[OBJCOPY]$< $@"
	arm-none-eabi-objcopy -O ihex $< $@

$(HEX): $(HEX_FW) $(HEX_FW_UPDATE)
	@echo "[SREC_CAT]   $@"
	srec_cat $(HEX_FW) -intel $(HEX_FW_UPDATE) -intel -o $@ -intel

$(BIN): $(HEX_FW)
	@echo "[SREC_CAT]  $@"
	srec_cat $? -intel -offset -0x8000000 -fill 0xff 0x00 0x7C00  -o $@ -binary
