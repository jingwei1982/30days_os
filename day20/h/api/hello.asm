[bits 32] 
     mov ecx,msg 
     mov edx,1

putloop:
     mov al,[cs:ecx]
     cmp al,0
     je fin 
     int 0x40 
     inc ecx
     jmp putloop

fin:
     retf 
msg:
     db "hello",0