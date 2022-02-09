boot_asm=ipl.asm
boot_bin=$(patsubst %.asm,%.bin,$(boot_asm))

loader_asm=loader.asm
loader_bin=$(patsubst %.asm,%.bin,$(loader_asm))

bin_wo_elf=$(boot_bin) $(loader_bin)

asms=asmhead.asm
asms_o=$(patsubst %.asm,%.o,$(asms))
asms_bin=$(patsubst %.asm,%.bin,$(asms))

kernel_bin=haribote.bin
target=wmy.img
bochs_conf=bochsrc.txt

$(target):$(kernel_bin) $(bin_wo_elf) empty.img
	cp empty.img $(target)
	dd if=$(boot_bin) of=$(target) bs=512 count=1 conv=notrunc
	sudo mount $(target) /media/ -t vfat -o loop
	sudo cp $(loader_bin) /media
	sudo cp $(kernel_bin) /media
	sudo sync
	@echo "Now the image include below files:"
	ls -alh /media
	sudo umount /media/
	@echo "Image created sucessfully! You can use make run to start the bochs!"

$(asms_o):%.o:%.asm
	@echo "Start to generate the .o file"
	nasm -f elf -g -F stabs $< -o $@

bootpack.o:bootpack.c
	gcc -m32 -c -o bootpack.o bootpack.c 

$(bin_wo_elf):%.bin:%.asm
	@echo "Dealing with "$<
	nasm $< -o $@

$(kernel_bin):$(asms_o) bootpack.o
	ld -m elf_i386 -Ttext 0x030400 -s $(asms_o) bootpack.o -o $(kernel_bin)

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
	-rm -rf ./*.bin
	-rm -rf $(target)
	-rm -rf ./*.dump
	-rm -rf ./*.o
	@echo "Clean finished!"
