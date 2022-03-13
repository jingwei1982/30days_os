CFLAGS		= -I include/ -c -ffreestanding

api_asm=$(wildcard ./*.asm)
api_asm_o=$(patsubst %.asm,%.o,$(api_asm))

api_c:=$(wildcard ./*.c)
api_c_o=$(patsubst %.c,%.o,$(api_c))
api_bin=$(patsubst %.c,%.bin,$(api_c))

$(api_bin):$(api_asm_o) $(api_c_o)
	@echo "Start to link the api "$@
	i386-elf-ld -nostdlib -Map=$(patsubst %.c,%.map,$(api_c)) -Ttext 0x8000 --oformat binary -s $(api_asm_o) $(api_c_o) -o $(api_bin)

$(api_asm_o):%.o:%.asm
	@echo "Start to generate the .o file from .asm in api folder" $<
	nasm $< -f elf -o $@ -l $(patsubst %.asm,%.lst,$(api_asm))

$(api_c_o):%.o:%.c	
	@echo "Start to generate the .o file from .c in api folder "$<
	i386-elf-gcc $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	@echo "Start to clean files."
	find . -type f -regex  ".*\.\(o\|bin\|map\|lst\)" | xargs rm -rf
	@echo "Clean finished!"

