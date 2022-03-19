extern HariMain;
[BITS 32]
    global api_putchar,api_end,api_putstr0
    global api_openwin,api_putstrwin,api_boxfilwin
	global api_initmalloc,api_malloc,api_free
	global api_point,api_refreshwin
	
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

	api_initmalloc:	; void api_initmalloc(void);
		; edx=8; ebx=memman address; eax: starting address; ecx: size.
		PUSH	EBX
		MOV		EDX,8
		MOV		EBX,[CS:0x0020]		; malloc内存空间的地址
		MOV		EAX,EBX
		ADD		EAX,32*1024				; 加上32KB
		MOV		ECX,[CS:0x0000]		; 数据段的大小
		SUB		ECX,EAX
		INT		0x40
		POP		EBX
		RET

	api_malloc:		; char *api_malloc(int size);
		; edx=9;  ebx=memman address; ecx=byte number required; eax=allocate address.
		PUSH	EBX
		MOV		EDX,9
		MOV		EBX,[CS:0x0020]
		MOV		ECX,[ESP+8]			; size
		INT		0x40
		POP		EBX
		RET

	api_free:			; void api_free(char *addr, int size);
		; edx=9;  ebx=memman address; eax=free memory's address; ecx=size number that will free.
		PUSH	EBX
		MOV		EDX,10
		MOV		EBX,[CS:0x0020]
		MOV		EAX,[ESP+ 8]		; addr
		MOV		ECX,[ESP+12]		; size
		INT		0x40
		POP		EBX
		RET

	api_point:		; void api_point(int win, int x, int y, int col);
	;edx=11, ebx=win handler, esi:x, edi:y, eax: color.
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,11
		MOV		EBX,[ESP+16]	; win
		MOV		ESI,[ESP+20]	; x
		MOV		EDI,[ESP+24]	; y
		MOV		EAX,[ESP+28]	; col
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET

	api_refreshwin:	; void api_refreshwin(int win, int x0, int y0, int x1, int y1);
	;edx=12,ebx=win handle, eax=x0,ecx=y0,esi=x1,edi=y1.
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		EDX,12
		MOV		EBX,[ESP+16]	; win
		MOV		EAX,[ESP+20]	; x0
		MOV		ECX,[ESP+24]	; y0
		MOV		ESI,[ESP+28]	; x1
		MOV		EDI,[ESP+32]	; y1
		INT		0x40
		POP		EBX
		POP		ESI
		POP		EDI
		RET