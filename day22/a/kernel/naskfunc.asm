; haribote-os boot asm
; TAB=4
; [map symbols naskfunc.map]
extern HariMain;
extern inthandler0d;
extern inthandler20;
extern inthandler21;
extern inthandler27;
extern inthandler2c;
extern cons_putchar;
extern hrb_api;

[section .data]
times 2048 db 0

[section .text]
	global 	_start
	global 	write_mem8
	global	io_hlt, io_cli, io_sti, io_stihlt
	global	io_in8,  io_in16,  io_in32
	global	io_out8, io_out16, io_out32
	global	io_load_eflags, io_store_eflags
	global	load_gdtr,load_idtr,load_tr, farjmp
	global  asm_inthandler0d
	global  asm_inthandler20,asm_inthandler21, asm_inthandler27, asm_inthandler2c
	global 	memtest,farcall
	global  asm_cons_putchar,asm_hrb_api
	global 	start_app

	_start:
		
		call 	HariMain

	io_hlt:	; void io_hlt(void);
			HLT
			RET

	io_cli:	; void io_cli(void);
			CLI
			RET

	io_sti:	; void io_sti(void);
			STI
			RET

	io_stihlt:	; void io_stihlt(void);
			STI
			HLT
			RET

	io_in8:	; int io_in8(int port);
			push 	ebp
			mov 	ebp, esp
			MOV		EDX,[EBP+8]		; port
			MOV		EAX,0
			IN		AL,DX
			POP		ebp
			RET

	io_in16:	; int io_in16(int port);
			push 	ebp
			mov 	ebp,esp
			MOV		EDX,[EBP+8]		; port
			MOV		EAX,0
			IN		AX,DX
			pop 	ebp
			RET

	io_in32:	; int io_in32(int port);
			push 	ebp
			mov 	ebp,esp
			MOV		EDX,[EBP+8]		; port
			IN		EAX,DX
			pop 	ebp
			RET

	io_out8:	; void io_out8(int port, int data);
			push 	ebp
			mov 	ebp,esp
			MOV		EDX,[EBP+8]		; port
			MOV		AL,[EBP+12]		; data
			OUT		DX,AL
			pop 	ebp
			RET

	io_out16:	; void io_out16(int port, int data);
			push 	ebp
			mov 	ebp,esp
			MOV		EDX,[EBP+8]		; port
			MOV		EAX,[EBP+12]		; data
			OUT		DX,AX
			pop 	ebp
			RET

	io_out32:	; void io_out32(int port, int data);
			push 	ebp
			mov 	ebp,esp
			MOV		EDX,[EBP+8]		; port
			MOV		EAX,[EBP+12]		; data
			OUT		DX,EAX
			pop 	ebp
			RET

	io_load_eflags:	; int io_load_eflags(void);
			PUSHFD		; PUSH EFLAGS 
			POP		EAX
			RET

	io_store_eflags:	; void io_store_eflags(int eflags);
			push 	ebp
			mov 	ebp, esp
			MOV		EAX,[EBP+8]
			PUSH	EAX
			POPFD		; POP EFLAGS 
			pop 	ebp
			RET

	load_gdtr:		; void load_gdtr(int limit, int addr);
			push 	ebp
			mov 	ebp,esp
			MOV		AX,[EBP+8]		; limit
			MOV		[EBP+10],AX
			LGDT	[EBP+10]
			pop 	ebp
			RET

	load_idtr:		; void load_idtr(int limit, int addr);
			push 	ebp
			mov 	ebp, esp
			MOV		AX,[ESP+8]		; limit
			MOV		[ESP+10],AX
			LIDT	[ESP+10]
			pop 	ebp
			RET

	load_tr:	;void load_tr(int tr); //让cpu记住当前正在运行哪个任务
			ltr [esp+4]
			ret

	farjmp:	;void farjmp(int eip,int cs);   
			jmp far [esp+4]	;jmp far指令会从指定的内存地址读取4个字节放入eip, 再继续读取2个字节放入cs。
			ret

	write_mem8:
			push 	ebp
			mov 	ebp, esp

			mov 	ECX,[EBP+8]
			mov 	AL,[EBP+12]
			mov 	[ECX],AL
	; 
			pop 	ebp
			RET

	asm_inthandler0d:
			push	es
			push	ds
			pushad
			mov eax,esp
			push eax
			mov ax,ss 
			mov ds,ax
			mov es,ax
			call inthandler0d
			cmp eax,0
			jne end_app
			pop eax
			popad
			pop ds
			pop es
			add esp,4
			iretd

	asm_inthandler20:
			push	es
			push 	ds
			pushad
			mov 	eax,esp
			push 	eax
			mov 	ax,ss 
			mov 	ds,ax
			mov 	es,ax
			call 	inthandler20
			pop 	eax
			popad
			pop 	ds
			pop 	es
			iretd


	asm_inthandler21:
			push	es
			push 	ds
			pushad
			mov 	eax,esp
			push 	eax
			mov 	ax,ss 
			mov 	ds,ax
			mov 	es,ax
			call 	inthandler21
			pop 	eax
			popad
			pop 	ds
			pop 	es
			iretd

	asm_inthandler27:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX,ESP
		PUSH	EAX
		MOV		AX,SS
		MOV		DS,AX
		MOV		ES,AX
		CALL	inthandler27
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD

	asm_inthandler2c:
			push	es
			push 	ds
			pushad
			mov 	eax,esp
			push 	eax
			mov 	ax,ss 
			mov 	ds,ax
			mov 	es,ax
			call 	inthandler2c
			pop 	eax
			popad
			pop 	ds
			pop 	es
			iretd

	memtest:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
			PUSH	EDI						; （由于还要使用EBX, ESI, EDI）
			PUSH	ESI
			PUSH	EBX
			MOV		ESI,0xaa55aa55			; pat0 = 0xaa55aa55;
			MOV		EDI,0x55aa55aa			; pat1 = 0x55aa55aa;
			MOV		EAX,[ESP+12+4]			; i = start;
	mts_loop:
			MOV		EBX,EAX
			ADD		EBX,0xffc				; p = i + 0xffc;
			MOV		EDX,[EBX]				; old = *p;
			MOV		[EBX],ESI				; *p = pat0;
			XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
			CMP		EDI,[EBX]				; if (*p != pat1) goto fin;
			JNE		mts_fin
			XOR		DWORD [EBX],0xffffffff	; *p ^= 0xffffffff;
			CMP		ESI,[EBX]				; if (*p != pat0) goto fin;
			JNE		mts_fin
			MOV		[EBX],EDX				; *p = old;
			ADD		EAX,0x1000				; i += 0x1000;
			CMP		EAX,[ESP+12+8]			; if (i <= end) goto mts_loop;
			JBE		mts_loop
			POP		EBX
			POP		ESI
			POP		EDI
			RET
	mts_fin:
			MOV		[EBX],EDX				; *p = old;
			POP		EBX
			POP		ESI
			POP		EDI
			RET

	asm_cons_putchar:
		sti			;use as interrupt call, the interrupt will disable, not necessary, so enable at this point.
		pushad
		push 1
		and eax,0xff
		push eax
		push DWORD [0xfec]
		call cons_putchar
		add esp,12	;drop data in stack.
		popad
		iretd

	farcall:	;void farcall(int eip,int cs);
		call far [esp+4]	;eip,cs
		ret

	asm_hrb_api:
		sti
		
		PUSH DS
		PUSH ES
		pushad 	;save value
		pushad 	;pass args to hrb_api

		mov ax,ss
		mov ds,ax
		mov es,ax

		call hrb_api

		cmp eax,0	
		jne end_app

		add esp,32
		popad
		pop es
		pop ds

		iretd	;this command will run sti
	end_app:
		;eax is the address of tss.esp0
		mov esp,[eax]
		popad
		ret	;back to cmd_app


	start_app:	;void start_app(int eip,int cs,int esp,int ds,int *tss_esp0);
		pushad

		mov eax,[esp+36]	;eip
		mov ecx,[esp+40]	;cs
		mov edx,[esp+44]	;esp
		mov ebx,[esp+48]	;ds/ss
		mov ebp,[esp+52]	
		mov [ebp],esp		;save os's esp
		mov [ebp+4],ss		;save os's ss

		mov es,bx
		mov ds,bx
		mov fs,bx
		mov gs,bx
		
		or ecx,3
		or ebx,3
		push ebx	;eds
		push edx	;esp
		push ecx	;cs
		push eax	;eip
		retf
		; will not return back to this point after the app terminate.


