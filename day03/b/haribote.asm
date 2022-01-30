;set cusror
	push 	DX
	mov dx,0100h
	call set_curser_pos
	pop DX

;display
	; push si
	; push cx
	; mov 	AX,0x0820
	; shl 	AX,4
	; MOV		SI,LoaderMsg
	; ADD     SI,AX
	; MOV 	CX,LoaderMsg_len
	; call	display
	; pop cx
	; pop si
; 打印L以确认进入了haribote。
	push	ax
	push	bx
	mov	ah,	0eh
	mov	al,	'L'
	mov	bl,	0fh
	int	10h
	pop	bx
	pop	ax

    fin:
    HLT
    JMP fin

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

LoaderMsg:	
	db "ABCDEFGHI"
LoaderMsg_len equ $-LoaderMsg
