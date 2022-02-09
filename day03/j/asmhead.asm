; haribote-os boot asm
; TAB=4
extern HariMain;

[section .text]
	global _start
	global io_hlt

	_start:
		
		call 	HariMain

	io_hlt:
		HLT
		RET
