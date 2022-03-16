asm=$(wildcard ./*.asm)
asm_bin=$(patsubst %.asm,%.bin,$(asm))
boot_bin=ipl.bin
target=wmy.img
bochs_conf=bochsrc.txt

$(target):$(asm_bin)  
	dd if=/dev/zero of=$(target) bs=512 count=2880
	dd if=$(boot_bin) of=$(target) bs=512 count=1 conv=notrunc
	@echo "Image created sucessfully! You can use make run to start the bochs!"

%.bin:%.asm
	@echo "Dealing with "$<
	nasm $< -o $@

run:$(target)
	bochs -f $(bochs_conf)

image:$(target)

.PHONY:clean
clean:
	-rm -rf $(asm_bin)
	-rm -rf $(target)
	@echo "Clean finished!"
