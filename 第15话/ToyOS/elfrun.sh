clang Kernel.c Video.c Shell.c -c -nostdlib -ffreestanding 
ld.lld Kernel.o Video.o Shell.o -e KernelStart -o Kernel.elf
cp ~/ToyOS/Kernel.elf ~/EDK/ovmf/esp/Kernel.elf
qemuovmf