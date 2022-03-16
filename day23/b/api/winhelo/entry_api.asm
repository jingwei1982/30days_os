extern HariMain;
[BITS 32]
    global api_putchar,api_end,api_putstr0
    global api_openwin
[section .text]
    call HariMain;
    retf

    api_openwin:    ;int api_openwin(char *buf,int xsiz,int ysiz,int col_inv,char *title)
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,5
		MOV		EBX,[ESP+16]	; buf
		MOV		ESI,[ESP+20]	; xsiz
		MOV		EDI,[ESP+24]	; ysiz
		MOV		EAX,[ESP+28]	; col_inv
		MOV		ECX,[ESP+32]	; title
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET


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
