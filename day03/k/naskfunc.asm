[section .data]	; 数据在此
db 0
[section .text]	; 代码在此

global io_hlt	; 我们必须导出 _start 这个入口，以便让链接器识别

io_hlt:
    HLT
    RET