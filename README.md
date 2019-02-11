# matrix4-fw

TODO better README

# Compilation

Required packages on Debian systems: make, gcc-arm-none-eabi, srec-cat 

build process:

cd Drivers/wiznet_drier && make

cd Drivers/STM32_HAL && make

cd . && make

outputs: 

build/mueb_fw_for_stlink.hex <-- to be flashed with debugger
build/mueb_fw_for_remote_update.bin <-- to be used for remote firmware update

# Debug in GNU environment

```bash
openocd -f interface/stlink-v2.cfg -c "hla_vid_pid 0x0483 0x3748" -f target/stm32f0x_stlink.cfg &
arm-none-eabi-gdb MUEB_fw.elf
```

```
target remote localhost:3333
monitor halt
load
//monitor reset
run
```
