; haribote-os boot asm
; TAB=4
extern HariMain;
extern inthandler06;
extern inthandler08;
extern inthandler0c;
extern inthandler21;
extern inthandler27;
extern inthandler2c;

[section .data]
times 2048 db 0

[section .text]
	global 	_start
	global 	write_mem8
	global	io_hlt, io_cli, io_sti, io_stihlt
	global	io_in8,  io_in16,  io_in32
	global	io_out8, io_out16, io_out32
	global	io_load_eflags, io_store_eflags
	global	load_gdtr,load_idtr
	global  asm_inthandler06, asm_inthandler08, asm_inthandler0c
	global  asm_inthandler21, asm_inthandler27, asm_inthandler2c


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

	write_mem8:
			push 	ebp
			mov 	ebp, esp

			mov 	ECX,[EBP+8]
			mov 	AL,[EBP+12]
			mov 	[ECX],AL
	; 
			pop 	ebp
			RET

	asm_inthandler06:
			push	es
			push 	ds
			pushad
			mov 	eax,esp
			push 	eax
			mov 	ax,ss 
			mov 	ds,ax
			mov 	es,ax
			call 	inthandler06
			pop 	eax
			popad
			pop 	ds
			pop 	es
			iretd

	asm_inthandler08:
			push	es
			push 	ds
			pushad
			mov 	eax,esp
			push 	eax
			mov 	ax,ss 
			mov 	ds,ax
			mov 	es,ax
			call 	inthandler08
			pop 	eax
			popad
			pop 	ds
			pop 	es
			iretd

	asm_inthandler0c:
			push	es
			push 	ds
			pushad
			mov 	eax,esp
			push 	eax
			mov 	ax,ss 
			mov 	ds,ax
			mov 	es,ax
			call 	inthandler0c
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
