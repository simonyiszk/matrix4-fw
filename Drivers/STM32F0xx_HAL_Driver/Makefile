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

TARGET     = hal

# Your C files from the /src directory
HAL_MODULES = gpio.c pwr_ex.c i2c_ex.c flash_ex.c rcc_ex.c rcc.c cortex.c flash.c tim_ex.c spi_ex.c tim.c dma.c pwr.c i2c.c

LL_MODULES = usart.c dma.c spi.c utils.c gpio.c exti.c rcc.c

SRCS       = $(addprefix stm32f0xx_hal_,$(HAL_MODULES)) $(addprefix stm32f0xx_ll_,$(LL_MODULES)) stm32f0xx_hal.c

# Basic HAL libraries

# that's it, no need to change anything below this line!

###############################################################################
# Toolchain

PREFIX     = arm-none-eabi
CC         = $(PREFIX)-gcc
AR         = $(PREFIX)-ar
OBJCOPY    = $(PREFIX)-objcopy
OBJDUMP    = $(PREFIX)-objdump
SIZE       = $(PREFIX)-size

###############################################################################
# Defines
DEFS       = -DSTM32F030x8 -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER

# Include search paths (-I)
INCS       = -IInc -IInc_CMSIS -IInc_STM

# Compiler flags
CFLAGS     = -std=gnu11 -c -Os
CFLAGS    += -mcpu=cortex-m0 -mthumb
CFLAGS    += -ffunction-sections -fdata-sections -fstack-usage
CFLAGS    += $(INCS) $(DEFS)

OBJS       = $(addprefix obj/,$(SRCS:.c=.o))

# Prettify output
V = 0
ifeq ($V, 0)
	Q = @
	P = > /dev/null
endif

###################################################

.PHONY: all clean

all: archive

objf:
	$Qmkdir -p obj

obj/%.o: Src/%.c  | objf
	@echo "[CC]      $(notdir $<)"
	$Q$(CC) $(CFLAGS) -c -o $@ $< 


archive:  $(OBJS)
	@echo "[AR]      $(TARGET).a"
	$Q$(AR) qco $(TARGET).a $(OBJS)

clean:
	@echo "[RM]      $(TARGET).a"; rm -f $(TARGET).a
	@echo "[RMDIR]   obj" ; rm -fr obj
