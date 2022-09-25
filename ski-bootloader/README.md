`ski-bootloader` is a boot loader for Ski to load ia64 linux ELF kernels
in vmlinux format. It's implemented as a firmware to set initial CPU
state close to how EFI would set it up before handing control off to
loaded kernel.

Currently kernel loading and console output relies on `Ski`-specifc
"syscalls". Eventually it will switch to use standard emulated devices
(like UART8250).

# Current status

`ski-bootloader` still relies on `Ski`-specific "syscalls" and can boot
kernel up to `linux-4.19`.

# Supported options

`ski-bootloader` understands some of the parameters passed to `ski`:

- first argument is interpreted as a kernel file name. `vmlinux` is
  used if no parameters are present.

# How to build

The build requires you to have **ia64-unknown-linux-gnu** toolchain:
**gcc** and **binutils**. Once you have it's it's just a matter of
running 'make':

    $ make

Whis will build you a static IA64 firmware in `linux-bootloader` file.

# How to run

Once you got `linux-bootloader` you can run `Ski`:

    $ bski ./linux-bootloader linux-4.19.241/vmlinux ...

# Bit of history

`ski-bootloader` started off as a `linux-4.19.241` `bootloader` code
from `arch/ia64/hp/sim/boot` (and a few supporting headers). Original
`bootlaoder` code was removed in `linux-5.0`. Moving it to a separate
location closer to `Ski` should allow extending `Ski` to boot unmodified
`linux` kernels.