BITS 64 
mov ecx, 0x0013C680
mov eax, 0xFF18679A
xor rdi, rdi
mov rdi, 0xC0000000

Write:
    mov [rdi], eax
    add rdi, 4
    loop Write

jmp $


