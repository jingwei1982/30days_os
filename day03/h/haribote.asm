;set cusror
	push 	DX
	mov dx,0100h
	call set_curser_pos
	pop DX

; 打印L以确认进入了haribote。
	push	ax
	push	bx
	mov	ah,	0eh
	mov	al,	'L'
	mov	bl,	0fh
	int	10h
	pop	bx
	pop	ax

; haribote-os boot asm
; TAB=4

BOTPAK	EQU		0x00280000		; 加载bootpack
DSKCAC	EQU		0x00100000		; 磁盘缓存的位置
DSKCAC0	EQU		0x00008000		; 磁盘缓存的位置（实模式）

; BOOT_INFO相关
CYLS	EQU		0x0ff0			; 引导扇区设置
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 关于颜色的信息
SCRNX	EQU		0x0ff4			; 分辨率X
SCRNY	EQU		0x0ff6			; 分辨率Y
VRAM	EQU		0x0ff8			; 图像缓冲区的起始地址

		MOV		AL,0x13			; VGA显卡，320x200x8bit
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 屏幕的模式（参考C语言的引用）
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; 通过BIOS获取指示灯状态

		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL


    fin:
    HLT
    JMP fin


;=======	set focus
;usage: step1, mov dx,0000h, step2, call set_cursor_pos
set_curser_pos:
		push 	AX
		push 	BX
		mov		AX,	0200h
		mov		BX,	0000h
		int		10h
		pop 	BX
		pop		AX
		ret 
