extern HariMain;
[BITS 32]
    global api_putchar,api_end,api_putstr0
[section .text]
    call HariMain;
    retf
    
    api_putchar:    ;void api_putchar(int c);
        mov edx,1
        mov al,[esp+4]  ;c
        int 0x40 
        ret

    api_end:    ;void api_end(void);
        mov edx,4
        int 0x40

    api_putstr0:    ;void api_putstr0(char *s)
        push ebx
        mov edx,2
        mov ebx,[esp+8] ;s
        int 0x40
        pop ebx
        ret
