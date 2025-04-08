`ski-efi` is a minimal EFI for Ski to load EFI binaries
in PE32+ format.

Currently kernel loading and console output relies on `Ski`-specifc
"syscalls". Eventually it will switch to use standard emulated devices
(like UART8250).

# Current status

`ski-efi` still relies on `Ski`-specific "syscalls" and can boot
`GRUB` which in turn can load kernel up to `linux-4.19`.

# Supported options

`ski-efi` understands some of the parameters passed to `ski`:

- first argument is interpreted as an EFI file name. `vmlinux` is
  used if no parameters are present.

# How to build

The build requires you to have **ia64-unknown-linux-gnu** toolchain:
**gcc** and **binutils**. Once you have it's it's just a matter of
running 'make':

    $ make

Whis will build you a static IA64 firmware in `ski-efi` file.

# How to run

Once you got `ski-efi` you can run `Ski`:

    $ grub-mkstandalone -o grub.efi -O ia64-efi
    $ bski ./ski-efi grub.efi

# Bit of history

`ski-efi` was developped in order to test ia64 GRUB in a virtual
environment.
