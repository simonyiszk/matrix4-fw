# matrix4-fw

TODO better README

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
