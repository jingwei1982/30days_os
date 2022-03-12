api_asm=$(wildcard ./*.asm)
api_asm_bin=$(patsubst %.asm,%.bin,$(api_asm))

$(api_asm_bin):%.bin:%.asm
	@echo "Start to generate the .o file from .asm in api folder" $<
	nasm $< -o $@

