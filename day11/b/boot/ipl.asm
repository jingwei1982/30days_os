; hello-os
; TAB=4

		ORG		0x7c00			; 指明程序装载地址

		JMP		entry
		DB 0x90
		%include "fat12hdr.inc"
		%include "load.inc"

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,BaseOfStack
		MOV		DS,AX
		MOV		ES,AX

;=======	clear screen
		mov		AX,	0600h
		mov		BX,	0700h
		mov		CX,	0
		mov		DX,	0184fh
		int		10h	

;set cusror
		push 	DX
		mov dx,0000h
		call set_curser_pos
		pop DX

;display
		push si
		push cx
		MOV		SI,init_msg
		MOV 	CX,init_msg_len
		call	display
		pop cx
		pop si

;=======	search loader.bin
	mov	word	[SectorNo],	SectorNumOfRootDirStart

Lable_Search_In_Root_Dir_Begin:

	cmp	word	[RootDirSizeForLoop],	0
	jz	Label_No_LoaderBin
	dec	word	[RootDirSizeForLoop]	
	mov	ax,	00h
	mov	es,	ax
	mov	bx,	8000h
	mov	ax,	[SectorNo]
	mov	cl,	1
	call	Func_ReadOneSector
	mov	si,	LoaderFileName
	mov	di,	8000h
	cld
	mov	dx,	10h
	
Label_Search_For_LoaderBin:

	cmp	dx,	0
	jz	Label_Goto_Next_Sector_In_Root_Dir
	dec	dx
	mov	cx,	11

Label_Cmp_FileName:

	cmp	cx,	0
	jz	Label_FileName_Found
	dec	cx
	lodsb	
	cmp	al,	byte	[es:di]
	jz	Label_Go_On
	jmp	Label_Different

Label_Go_On:
	
	inc	di
	jmp	Label_Cmp_FileName

Label_Different:

	and	di,	0ffe0h
	add	di,	20h
	mov	si,	LoaderFileName
	jmp	Label_Search_For_LoaderBin

Label_Goto_Next_Sector_In_Root_Dir:
	
	add	word	[SectorNo],	1
	jmp	Lable_Search_In_Root_Dir_Begin
	
;=======	display on screen : ERROR:No LOADER Found


Label_No_LoaderBin:
	;set cusror
	push 	DX
	mov dx,0100h
	call set_curser_pos
	pop DX


	mov SI,NoLoaderMessage
	mov cx, NoLoaderMessage_LEN
	jmp display
	jmp	fin

;=======	found loader.bin name in root director struct

Label_FileName_Found:

	mov	ax,	RootDirSectors
	and	di,	0ffe0h
	add	di,	01ah
	mov	cx,	word	[es:di]
	push	cx
	add	cx,	ax
	add	cx,	SectorBalance
	mov	ax,	BaseOfLoader
	mov	es,	ax
	mov	bx,	OffsetOfLoader
	mov	ax,	cx

Label_Go_On_Loading_File:
	mov	cl,	1
	call	Func_ReadOneSector
	pop	ax
	call	Func_GetFATEntry
	cmp	ax,	0fffh
	jz	Label_File_Loaded
	push	ax
	mov	dx,	RootDirSectors
	add	ax,	dx
	add	ax,	SectorBalance
	add	bx,	[BPB_BytesPerSec]
	jmp	Label_Go_On_Loading_File

Label_File_Loaded:
	
	jmp	BaseOfLoader:OffsetOfLoader

;=======	read one sector from floppy

Func_ReadOneSector:
	
	push	bp
	mov	bp,	sp
	sub	esp,	2
	mov	byte	[bp - 2],	cl
	push	bx
	mov	bl,	[BPB_SecPerTrk]
	div	bl
	inc	ah
	mov	cl,	ah
	mov	dh,	al
	shr	al,	1
	mov	ch,	al
	and	dh,	1
	pop	bx
	mov	dl,	[BS_DrvNum]
Label_Go_On_Reading:
	mov	ah,	2
	mov	al,	byte	[bp - 2]
	int	13h
	jc	Label_Go_On_Reading
	add	esp,	2
	pop	bp
	ret

;=======	get FAT Entry

Func_GetFATEntry:

	push	es
	push	bx
	push	ax
	mov	ax,	00
	mov	es,	ax
	pop	ax
	mov	byte	[Odd],	0
	mov	bx,	3
	mul	bx
	mov	bx,	2
	div	bx
	cmp	dx,	0
	jz	Label_Even
	mov	byte	[Odd],	1

Label_Even:

	xor	dx,	dx
	mov	bx,	[BPB_BytesPerSec]
	div	bx
	push	dx
	mov	bx,	8000h
	add	ax,	SectorNumOfFAT1Start
	mov	cl,	2
	call	Func_ReadOneSector
	
	pop	dx
	add	bx,	dx
	mov	ax,	[es:bx]
	cmp	byte	[Odd],	1
	jnz	Label_Even_2
	shr	ax,	4

Label_Even_2:
	and	ax,	0fffh
	pop	bx
	pop	es
	ret

		; display用法，step1, mov SI,msg_address, step2, mov CX,length
display:
		push 	AX
		push 	BX

next_char_display:
		MOV		AL,[SI]
		ADD		SI,1			; 给SI加1
		MOV		AH,0x0e			; 显示一个文字
		MOV		BX,15			; 指定字符颜色
		INT		0x10			; 调用显卡BIOS
		loop next_char_display

		pop 	BX
		pop 	AX
		ret

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

fin:
		HLT						; 让CPU停止，等待指令
		JMP		fin				; 无限循环

;=======	tmp variable

RootDirSizeForLoop	dw	RootDirSectors
SectorNo		dw	0
Odd			db	0

;=======	display messages

NoLoaderMessage:	db	"ERROR:No LOADER Found"
NoLoaderMessage_LEN equ $-NoLoaderMessage
LoaderFileName:		db	"LOADER  BIN",0

init_msg:
		DB		"start boot"
		DB		0x0a			; 换行
		init_msg_len equ $-init_msg


		times 0x1fe-($-$$) db 0		; 填写0x00直到0x001fe

		DB		0x55, 0xaa

