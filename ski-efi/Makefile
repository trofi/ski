SKI_BOOTLOADER_OBJS = \
	boot_head.o \
	bootloader.o \
	fw-emu.o \
	memstr.o

ski-efi: $(SKI_BOOTLOADER_OBJS) bootloader.lds
	$(LD) $(SKI_BOOTLOADER_OBJS) -o $@ -static -T bootloader.lds

CROSS_COMPILE = ia64-unknown-linux-gnu-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

CFLAGS  = \
	-nostdinc -Iinclude \
	-ffixed-r13 -mfixed-range=f12-f15,f32-f127 -mconstant-gp -g \
	-falign-functions=32 -frename-registers -fno-optimize-sibling-calls -fno-builtin

ASFLAGS = \
	-nostdinc -Iinclude \
	\
	-mconstant-gp \
	-Wa,-gdwarf-2
