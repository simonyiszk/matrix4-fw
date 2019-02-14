# matrix4-fw

TODO better README

# Compilation

Tested on **Debian** with gcc-arm-none-eabi/stable,now 15:5.4.1+svn241155-1 amd64

Required packages on Debian systems: **make**, **gcc-arm-none-eabi**, **srecord**

build process:

simply run **make**

outputs: 

build/MUEB_for_stlink_flash.hex <-- to be flashed with debugger
build/MUEB_for_remote_update.bin <-- to be used for remote firmware update

# Debug in GNU environment

```bash
openocd -f interface/stlink-v2.cfg -c "hla_vid_pid 0x0483 0x3748" -f target/stm32f0x.cfg & arm-none-eabi-gdb MUEB_fw.elf --eval-command="target remote localhost:3333"
```

```
monitor halt
load
monitor reset
run
```
