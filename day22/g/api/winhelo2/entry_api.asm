extern HariMain;
[BITS 32]
    global api_putchar,api_end,api_putstr0
    global api_openwin,api_putstrwin,api_boxfilwin
[section .text]
    call HariMain;
    retf

    api_openwin:    ;int api_openwin(char *buf,int xsiz,int ysiz,int col_inv,char *title)
		; edx=5; ebx=win buffer; esi=x size; edi=y size; eax=col_inv; ecx=windows' title.
		; eax is the returned value, eax:sheet handler.
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

    api_putstrwin:	; void api_putstrwin(int win, int x, int y, int col, int len, char *str);
		;edx=6;ebx=windows handler;esi=x; edi=y; eax=color; ecx=string's length; ebp=string
		PUSH	EDI
		PUSH	ESI
		PUSH	EBP
		PUSH	EBX
		MOV		EDX,6
		MOV		EBX,[ESP+20]	; win
		MOV		ESI,[ESP+24]	; x
		MOV		EDI,[ESP+28]	; y
		MOV		EAX,[ESP+32]	; col
		MOV		ECX,[ESP+36]	; len
		MOV		EBP,[ESP+40]	; str
		INT		0x40
		POP		EBX
		POP		EBP
		POP		ESI
		POP		EDI
		RET

    api_boxfilwin:	; void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
		PUSH	EDI
		PUSH	ESI
		PUSH	EBP
		PUSH	EBX
		MOV		EDX,7
		MOV		EBX,[ESP+20]	; win
		MOV		EAX,[ESP+24]	; x0
		MOV		ECX,[ESP+28]	; y0
		MOV		ESI,[ESP+32]	; x1
		MOV		EDI,[ESP+36]	; y1
		MOV		EBP,[ESP+40]	; col
		INT		0x40
		POP		EBX
		POP		EBP
		POP		ESI
		POP		EDI
		RET
