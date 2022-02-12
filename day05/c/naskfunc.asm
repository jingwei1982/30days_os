; haribote-os boot asm
; TAB=4
extern HariMain;

[section .text]
	global _start
	global write_mem8
	global	io_hlt, io_cli, io_sti, io_stihlt
	global	io_in8,  io_in16,  io_in32
	global	io_out8, io_out16, io_out32
	global	io_load_eflags, io_store_eflags


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

	write_mem8:
			push 	ebp
			mov 	ebp, esp

			mov 	ECX,[EBP+8]
			mov 	AL,[EBP+12]
			mov 	[ECX],AL
	; 
			pop 	ebp
			RET
