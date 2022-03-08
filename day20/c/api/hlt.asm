[bits 32]
     mov al,'A'
     call 2*8:0x30541
     ; CLI
fin:
    hlt
    jmp fin