; haribote-os boot asm
; TAB=4
extern HariMain;

[section .text]
	global _start
	global io_hlt
	global write_mem8

	_start:
		
		call 	HariMain

	io_hlt:
		HLT
		RET

; 	write_mem8:
; 		push 	ebp
; 		mov 	ebp, esp

; 		mov 	ECX,[EBP+8]
; 		mov 	AL,[EBP+12]
; 		mov 	[ECX],AL
; ; 
; 		pop 	ebp
; 		RET

	write_mem8:
		; push 	ebp
		; mov 	ebp, esp

		mov 	ECX,[ESP+4]
		mov 	AL,[ESP+8]
		mov 	[ECX],AL
; 
		; pop 	ebp
		RET

