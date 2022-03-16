asm=$(wildcard ./*.asm)
asm_bin=$(patsubst %.asm,%.bin,$(asm))
boot_bin=ipl.bin
target=wmy.img
bochs_conf=bochsrc.txt

$(target):$(asm_bin) empty.img
	dd if=/dev/zero of=$(target) bs=512 count=2880
	dd if=$(boot_bin) of=$(target) bs=512 count=1 conv=notrunc
	sudo mount $(target) /media/ -t vfat -o loop
	sudo cp $(asm_bin) /media
	sudo rm -rf /media/$(boot_bin)
	sudo sync
	@echo "Now the image include below files:"
	ls -alh /media
	sudo umount /media/
	@echo "Image created sucessfully! You can use make run to start the bochs!"

%.bin:%.asm
	@echo "Dealing with "$<
	nasm $< -o $@

run:$(target)
	bochs -f $(bochs_conf)

image:$(target)

dump:$(target)
	@echo "start to dump image file."
	dd if=$(target) of=fat12.dump skip=1 bs=512 count=18 
	dd if=$(target) of=boot_dir.dump skip=19 bs=512 count=14
	dd if=$(target) of=boot.dump skip=0 bs=512 count=1
	ls -al |grep .dump
	@echo "finish to dump image."

.PHONY:clean
clean:
	-rm -rf $(asm_bin)
	-rm -rf $(target)
	-rm -rf ./*.dump
	@echo "Clean finished!"
