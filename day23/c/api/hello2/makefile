api_asm=$(wildcard ./*.asm)
api_asm_o=$(patsubst %.asm,%.o,$(api_asm))
api_asm_bin=$(patsubst %.asm,%.bin,$(api_asm))


$(api_asm_bin):$(api_asm_o)
	@echo "Start to link the api "$@
	i686-elf-ld -nostdlib -Ttext 0x8000 --oformat binary -s $(api_asm_o) -o $(api_asm_bin)

$(api_asm_o):%.o:%.asm
	@echo "Start to generate the .o file from .asm in api folder" $<
	nasm $< -f elf -o $@

