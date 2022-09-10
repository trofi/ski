ski-bootloader: boot_head.o bootloader.o fw-emu.o bootloader.lds
	$(LD) boot_head.o bootloader.o fw-emu.o -o $@ -static -T bootloader.lds

CROSS_COMPILE = ia64-unknown-linux-gnu-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

CFLAGS  = \
	-nostdinc -Iinclude \
	-ffixed-r13 -mfixed-range=f12-f15,f32-f127 -mconstant-gp \
	-falign-functions=32 -frename-registers -fno-optimize-sibling-calls -fno-builtin

ASFLAGS = \
	-nostdinc -Iinclude \
	\
	-mconstant-gp \
	-Wa,-gdwarf-2
