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

TOOLCHAIN             := arm-none-eabi
CC                    := $(TOOLCHAIN)-gcc
CXX                   := $(TOOLCHAIN)-g++
AS                    := $(TOOLCHAIN)-as
OBJCOPY	              := $(TOOLCHAIN)-objcopy
SIZE 		      := $(TOOLCHAIN)-size
C_STANDARD	      := -std=gnu11
CXX_STANDARD 	      := -std=gnu++11

DEFS                  := -DSTM32F030x8 -DUSE_HAL_DRIVER  -DUSE_FULL_LL_DRIVER

COMMON_COMPILER_FLAGS := -O2 -g -fstack-usage -Wall -Wextra -Wpacked -Winline  -mcpu=cortex-m0 -mthumb
C_FLAGS               := $(COMMON_COMPILER_FLAGS) $(C_STANDARD)
CXX_FLAGS             := $(COMMON_COMPILER_FLAGS) $(CXX_STANDARD) -fno-rtti -fno-exceptions -fno-threadsafe-statics 

LD_FLAGS              := -specs=nano.specs

#TODO script this
INCLUDES               = -I Inc
INCLUDES              += -isystem Drivers/CMSIS/Device/ST/STM32F0xx/Include -isystem Drivers/CMSIS/Include -IDrivers/STM32F0xx_HAL_Driver/Inc
INCLUDES              += -isystem Drivers/ioLibrary_Driver/Internet/DHCP -IDrivers/ioLibrary_Driver/Ethernet -IDrivers/ioLibrary_Driver/Ethernet/W5500


C_FILES               := mac_eeprom.c dhcp_buffer.c stm32f0xx_it.c system_stm32f0xx.c main.c

CPP_FILES             := internal_anim.cpp firm_update.cpp main2.cpp network.cpp window.cpp

####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####

C_OBJS                 = $(addprefix build/c_,$(C_FILES:.c=.o))
CPP_OBJS               = $(addprefix build/cpp_,$(CPP_FILES:.cpp=.o))


all: build_dir $(C_OBJS) $(CPP_OBJS)

.PHONY: build_dir clean

build_dir: 
	@mkdir -p build

build/c_%.o: Src/%.c | build_dir
	@echo "[CC]	$(notdir $<)"
	$(CC) $(C_FLAGS) $(DEFS) $(INCLUDES) -c -o $@ $<

build/cpp_%.o: Src/%.cpp | build_dir
	@echo "[CXX]	$(notdir $<)"
	$(CXX) $(CXX_FLAGS) $(DEFS) $(INCLUDES) -c -o $@ $<

clean:
	@echo "[RM]     $(C_OBJS) $(CPP_OBJS) firmware.elf"
	@rm -f $(C_OBJS) $(CPP_OBJS) firmware.elf

firmware.elf: $(C_OBJS) $(CPP_OBJS)
	@echo "[LD]     $@"
	$(CXX) -o $@ Drivers/wiznet_driver/ioLibrary.a Drivers/STM32F0xx_HAL_Driver/hal.a $< $(CXX_FLAGS) $(LD_FLAGS)
