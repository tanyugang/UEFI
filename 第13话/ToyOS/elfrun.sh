clang Kernel.c -nostdlib -ffreestanding -o Kernel.elf
cp ~/ToyOS/Kernel.elf ~/EDK/ovmf/esp/Kernel.elf
qemuovmf