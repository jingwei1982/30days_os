vpath=boot/include include

boot_asm=boot/ipl.asm
boot_bin=$(patsubst %.asm,%.bin,$(boot_asm))
boot_include=$(wildcard boot/include/*.inc)

loader_asm=boot/loader.asm
loader_bin=$(patsubst %.asm,%.bin,$(loader_asm))

bin_wo_elf=$(boot_bin) $(loader_bin)

asms=kernel/naskfunc.asm
asms_o=$(patsubst %.asm,%.o,$(asms))
asms_bin=$(patsubst %.asm,%.bin,$(asms))

c:=$(wildcard kernel/*.c)
c_o=$(patsubst %.c,%.o,$(c))
c_include=$(wildcard include/*.h)

ASMBFLAGS	= -I boot/include/
CFLAGS		= -I include/ -c -m32

kernel_bin=haribote.bin
target=wmy.img
bochs_conf=bochsrc.txt

$(target):$(kernel_bin) $(bin_wo_elf)  
	dd if=/dev/zero of=$(target) bs=512 count=2880
	dd if=$(boot_bin) of=$(target) bs=512 count=1 conv=notrunc
	sudo mount $(target) /media/ -t vfat -o loop
	sudo cp $(loader_bin) /media
	sudo cp $(kernel_bin) /media
	sudo sync
	@echo "Now the image include below files:"
	ls -alh /media
	sudo umount /media/
	@echo "Image created sucessfully! You can use make run to start the bochs!"

$(bin_wo_elf):%.bin:%.asm $(boot_include)
	@echo "Dealing with "$<
	nasm $(ASMBFLAGS) $< -o $@

$(asms_o):%.o:%.asm
	@echo "Start to generate the " $<
	nasm -f elf -g -F stabs $< -o $@

$(c_o):%.o:%.c $(c_include)
	@echo "Dealing with "$<
	gcc $(CFLAGS) $< -o $@

$(kernel_bin):$(asms_o) $(c_o)
	@echo "Start to ld the kernel file."
	ld -m elf_i386 -Ttext 0x030400 -s $(asms_o) $(c_o) -o $(kernel_bin)

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
	@echo "Start to clean files."
	-rm -rf ./*.bin
	-rm -rf $(target)
	-rm -rf ./*.dump
	-rm -rf ./*.o
	-rm -rf ./*.log
	-rm -rf ./boot/*.bin
	-rm -rf ./kernel/*.o
	-rm -rf ./kernel/*.bin
	@echo "Clean finished!"
