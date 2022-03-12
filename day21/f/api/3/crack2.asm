[BITS 32]
[section .text]
    mov eax,1*8
    mov ds,ax
    mov byte[0x62600],0
    retf