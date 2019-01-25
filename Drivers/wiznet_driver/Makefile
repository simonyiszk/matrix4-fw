# STM32 Makefile for GNU toolchain and openocd
#
# This Makefile fetches the Cube firmware package from ST's' website.
# This includes: CMSIS, STM32 HAL, BSPs, USB drivers and examples.
#
# Usage:
#	make cube		Download and unzip Cube firmware
#	make program		Flash the board with OpenOCD
#	make openocd		Start OpenOCD
#	make debug		Start GDB and attach to OpenOCD
#	make dirs		Create subdirs like obj, dep, ..
#	make template		Prepare a simple example project in this dir
#
# Copyright	2015 Steffen Vogel
# License	http://www.gnu.org/licenses/gpl.txt GNU Public License
# Author	Steffen Vogel <post@steffenvogel.de>
# Link		http://www.steffenvogel.de
# Modfied by Kiss Adam 2019

TARGET     = ioLibrary

###############################################################################
# Toolchain

PREFIX     = arm-none-eabi
CC         = $(PREFIX)-gcc
AR         = $(PREFIX)-ar
OBJCOPY    = $(PREFIX)-objcopy
OBJDUMP    = $(PREFIX)-objdump
SIZE       = $(PREFIX)-size

###############################################################################
# Options

# Defines
DEFS       = -DSTM32F030x8 

# Include search paths (-I)
INCS       = -IioLibrary_Driver/Ethernet/ -IioLibrary_Driver/Ethernet/W5500 -Iconfigs

# Compiler flags
CFLAGS     = -std=gnu11 -c -Os
CFLAGS    += -mcpu=cortex-m0 -mthumb
CFLAGS    += -ffunction-sections -fdata-sections -fstack-usage
CFLAGS    += $(INCS) $(DEFS)

# Prettify output
V = 1
ifeq ($V, 0)
	Q = @
	P = > /dev/null
endif

###################################################

.PHONY: all clean

all: archive

objf:
	$Qmkdir -p obj

socket: ioLibrary_Driver/Ethernet/socket.c | objf
	@echo "[CC]      $(notdir $<)"
	$Q$(CC) $(CFLAGS) -c -o obj/$@.o $< 

w5500: ioLibrary_Driver/Ethernet/W5500/w5500.c | objf
	@echo "[CC]      $(notdir $<)"
	$Q$(CC) $(CFLAGS) -c -o obj/$@.o $< 

config: ioLibrary_Driver/Ethernet/wizchip_conf.c | objf
	@echo "[CC]      $(notdir $<)"
	$Q$(CC) $(CFLAGS) -c -o obj/$@.o $< 

dhcp: ioLibrary_Driver/Internet/DHCP/dhcp.c | objf
	@echo "[CC]      $(notdir $<)"
	$Q$(CC) $(CFLAGS) -c -o obj/$@.o $< 

archive:  socket w5500 config dhcp
	@echo "[AR]      $(TARGET).ar"
	$Q$(AR) qco $(TARGET).ar $(OBJS)

clean:
	@echo "[RM]      $(TARGET).ar"; rm -f $(TARGET).ar
	@echo "[RMDIR]   obj" ; rm -fr obj
