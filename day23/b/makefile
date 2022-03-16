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

api_sub_dir=$(addprefix api/,$(shell ls -l api | grep ^d |awk '{print $$9}'))

ASMBFLAGS	= -I boot/include/
CFLAGS		= -I include/ -c -ffreestanding

kernel_bin=haribote.bin
target=wmy.img
bochs_conf=bochsrc.txt

sub_makefile:
	@echo "Start to run the makefile in sub folder."
	for subdir in $(api_sub_dir);do \
		echo "";\
		echo "run makefile in $$subdir"; \
		(cd $$subdir && make); \
		echo "";\
	done

$(target):$(kernel_bin) $(bin_wo_elf) sub_makefile
	dd if=/dev/zero of=$(target) bs=512 count=2880
	dd if=$(boot_bin) of=$(target) bs=512 count=1 conv=notrunc
	sudo mount $(target) /media/ -t vfat -o loop
	sudo cp makefile /media
	for f_bin in $(shell find api -type f -regex ".*\.bin"); do\
		sudo cp $$f_bin /media;\
	done
	sudo cp $(bochs_conf) /media
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
	i386-elf-gcc $(CFLAGS) $< -o $@

$(kernel_bin):$(asms_o) $(c_o)
	@echo "Start to ld the kernel file."
	i386-elf-ld -nostdlib -Map=kernel.map -Ttext 0x030400 -s $(asms_o) $(c_o) -o $(kernel_bin)

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

reimage:
	make clean
	make image

rerun:
	make clean
	make run

.PHONY:clean
clean:
	@echo "Start to clean files."
	-find . -type f -regex  ".*\.\(o\|bin\|map\|lst\|\log\|dump\)" | xargs rm -rf
	-rm -rf $(target)
	@echo "Clean finished!"
