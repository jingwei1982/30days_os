; hello-os
; TAB=4

		ORG		0x7c00			; 指明程序装载地址
		BaseOfStack	equ	0x7c00

		BaseOfLoader	equ	0x2000
		OffsetOfLoader	equ	0x00

		RootDirSectors	equ	14
		SectorNumOfRootDirStart	equ	19
		SectorNumOfFAT1Start	equ	1
		SectorBalance	equ	17	

; 标准FAT12格式软盘专用的代码 Stand FAT12 format floppy code

		JMP		entry
		; DB		0x90
		; DB		"HELLOIPL"		; 启动扇区名称（8字节）
		; DW		512				; 每个扇区（sector）大小（必须512字节）
		; DB		1				; 簇（cluster）大小（必须为1个扇区）
		; DW		1				; FAT起始位置（一般为第一个扇区）
		; DB		2				; FAT个数（必须为2）
		; DW		224				; 根目录大小（一般为224项）
		; DW		2880			; 该磁盘大小（必须为2880扇区1440*1024/512）
		; DB		0xf0			; 磁盘类型（必须为0xf0）
		; DW		9				; FAT的长度（必??9扇区）
		; DW		18				; 一个磁道（track）有几个扇区（必须为18）
		; DW		2				; 磁头数（必??2）
		; DD		0				; 不使用分区，必须是0
		; DD		2880			; 重写一次磁盘大小
		; DB		0,0,0x29		; 意义不明（固定）
		; DD		0xffffffff		; （可能是）卷标号码
		; DB		"HELLO-OS   "	; 磁盘的名称（必须为11字?，不足填空格）
		; DB		"FAT12   "		; 磁盘格式名称（必??8字?，不足填空格）
		; TIMES	18	db 0		; 先空出18字节
		
		DB 0x90
		BS_OEMName	db	'MINEboot'			; 启动扇区名称（8字节）
		BPB_BytesPerSec	dw	512				; 每个扇区（sector）大小（必须512字节）
		BPB_SecPerClus	db	1				; 簇（cluster）大小（必须为1个扇区）
		BPB_RsvdSecCnt	dw	1				; FAT起始位置（一般为第一个扇区）
		BPB_NumFATs		db	2				; FAT个数（必须为2）
		BPB_RootEntCnt	dw	224				; 根目录大小（一般为224项）
		BPB_TotSec16	dw	2880			; 该磁盘大小（必须为2880扇区1440*1024/512）
		BPB_Media		db	0xf0			; 磁盘类型（必须为0xf0）
		BPB_FATSz16		dw	9				; FAT的长度（必??9扇区）
		BPB_SecPerTrk	dw	18				; 一个磁道（track）有几个扇区（必须为18）
		BPB_NumHeads	dw	2				; 磁头数（必??2）
		BPB_HiddSec		dd	0				; 不使用分区，必须是0
		BPB_TotSec32	dd	2880				; 重写一次磁盘大小
		BS_DrvNum		db	0				; 意义不明（固定）		
		BS_Reserved1	db	0				; 意义不明（固定）
		BS_BootSig		db	0x29			; 意义不明（固定）
		BS_VolID		dd	0				; （可能是）卷标号码
		BS_VolLab		db	'boot loader'	; 磁盘的名称（必须为11字?，不足填空格）
		BS_FileSysType	db	'FAT12   '		; 磁盘格式名称（必??8字?，不足填空格）

; 程序主体

entry:
		MOV		AX,0			; 初始化寄存器
		MOV		SS,AX
		MOV		SP,0x7c00
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
	; push	ax
	; push	bx
	; mov	ah,	0eh
	; mov	al,	'.'
	; mov	bl,	0fh
	; int	10h
	; pop	bx
	; pop	ax

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
LoaderFileName:		db	"HARIBOTEBIN",0

init_msg:
		DB		"start boot"
		DB		0x0a			; 换行
		init_msg_len equ $-init_msg


		times 0x1fe-($-$$) db 0		; 填写0x00直到0x001fe

		DB		0x55, 0xaa

