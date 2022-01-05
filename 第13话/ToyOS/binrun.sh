nasm -f bin Kernel.asm -o Kernel.bin
cp ~/ToyOS/Kernel.bin ~/EDK/ovmf/esp/Kernel.bin
qemuovmf