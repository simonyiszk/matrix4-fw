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

DEFS                  := -DSTM32F030x8 -DUSE_HAL_DRIVER 

COMMON_COMPILER_FLAGS := -O2 -g -fstack-usage -fno-threadsafe-statics -Wall -Wextra -Wpacked -Winline  -mcpu=cortex-m0 -mthumb
C_FLAGS               := $(COMMON_COMPILER_FLAGS) $(C_STANDARD)
CXX_FLAGS             := $(COMMON_COMPILER_FLAGS) $(CXX_STANDARD) -fno-rtti -fno-exceptions

LD_FLAGS              := -specs=nano.specs

#TODO script this
INCLUDES              := -I"/home/kisada/Atollic/TrueSTUDIO/STM32_workspace_9.0/matrix4_mueb_fw/Inc" -I"/home/kisada/Atollic/TrueSTUDIO/STM32_workspace_9.0/matrix4_mueb_fw/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"/home/kisada/Atollic/TrueSTUDIO/STM32_workspace_9.0/matrix4_mueb_fw/Drivers/CMSIS/Include" -I"/home/kisada/Atollic/TrueSTUDIO/STM32_workspace_9.0/matrix4_mueb_fw/Drivers/STM32F0xx_HAL_Driver/Inc"

C_FILES               := 

CPP_FILES             := 

####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####----####

build_dir: .PHONY
	mkdir -p build

c_sources: 

cpp_sources:

all: build_dir c_sources cpp_sources
