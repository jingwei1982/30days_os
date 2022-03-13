platfrom: ubuntu 20.04 LTS

Tools: nasm, gcc, dd, bochs

相对教程，最大的特点是不使用作者自己改写的编译器，而是使用常规的nasm和gcc/ld。我这里前20章用的都是64位的gcc， 21章开始自己编译了一个32位的cross toolchain, 能规避很多问题。链接如下，<https://wiki.osdev.org/GCC_Cross-Compiler#Why_cross-compilers_are_necessary>。 另外，kernel部分使用了elf格式，而api部分使用了 --oformat binary格式。

另外，我想说一句，尽量不要在国内网站搜索，实在太垃圾了，在baidu一星期解决不了的问题，在gg可能10分钟就搞清楚了。

Q: 如何运行？

A: 在ubuntu环境中，进入目录后，执行make run可以看到在虚拟机中运行的效果。

Q:如何查看bin文件？

A: xxd -a -u -g 1 xxx.bin或者hexdump -C xxx.bin

Q:如何查看obj文件？

A: objdump -d *.o


相对课本，目录基本和教材保持已知，主要改动如下：

day02a: 增加清屏和设置cursor。

day03a: display改成函数调用，添加读一个扇区的功能.已知问题，打印比较乱，回车后cursor没有回到最左边。

day03b: 教材上是固定写haribote.nas地址，然后读入固定的柱面，有很多局限性，现在ipl.asm中增加搜索haribote.bin的代码，会用到FAT和根目录搜索。碰到的问题，将haribote.bin放到0x820:0000会出错，但放在0x1000:0000却是正常的。

day03g: 进入haribote，打印了一个L验证正确进入了haribote。

day03g1: make run后会显示"SLDT: not recognized in real or virtual-8086 mode", 改变的是添加了[section .text]。

day03h: 和教材保持一致，但是去掉了org 0xc200,这是因为day03b的改动引起的。

day03j:加入了汇编和C混合编程，编译为elf格式，通过loader将kernel加载进内存，并跳转到Ttext入口。

day04a: 汇编和C互相调用，可以正常显示纯白屏幕。

day04f: 可以显示窗口。

day05b: 结构体。

day05e: 添加hankaku字体，直接添加在.h文件中。

day05f: 注意c语言中'和"的差异，在python中是一样，但c中一个是字符，一个是字符串，完全不同的东西。

day05g: 打印变量失败，错误如下，没搞定
ld -m elf_i386 -Ttext 0x030400 -s naskfunc.o bootpack.o -o haribote.bin
ld: bootpack.o: in function `HariMain':
bootpack.c:(.text+0x6d): undefined reference to `sprintf'
make: *** [makefile:41: haribote.bin] Error 1
在day07a中解决此问题。

day05f1：使用交叉编译器。

day05i: 设置GDT/IDT.

day06c: 调整目录架构。

day06d: PIC, 调整目录后，c文件无法跳转到.h文件，添加.vscode来解决此问题。
day06e: IDT设置卡了我好长时间，最终把dsctbl.c和loader.asm中GDT设置为一致后解决。
day07a: 重写sprintf，现在可以正常打印变量。
day07a1: 使用交叉编译。

day07b: 移除int 06/08/0c. 修改了boxfill长度设置。

day07c: FIFO.

day07f: mouse can work.

day08d：鼠标移动完成。

day09a: 整理文件夹

day09c: 检测内存size

day10a: can use 4K

day10d: 加速刷新速度

day11b: 设置sheet边界

day11c: shtctl的指定省略

day11d: 添加一个窗口。

day11f: 告诉计数器，需要把循环中的hlt去除。现在的问题是刷新时候背景闪烁。

day11g: 只修改的图层刷新避免闪烁。

day11h: sheet.c有问题，直接复制。

day12a:定时器

day12c: bochs中定时器会快10倍，但在实际系统中没有问题，还没找到原因。通过在bochsrc.txt中设置"clock: sync=realtime, time0=local, rtc_sync=0"可以解决。这里sync默认用的是none.

day12d: 更加深刻的理解指针。现在的中断处理花费时间过长。

day12g: 运行黑屏，发现是init_pic中using没有初始化导致的。

day13a: 简化代码.h中， 函数声明，函数参数使用一个后边才定义的结构体，结果不停报错。更改顺序后，正常。

day13b: timer fifo改为1个。

day13c: 模拟器上counter分别为：2271875，2879104，2880989，2940833. 虚拟机上为542013188、556669675，546516418.

day13g：模拟器上counter分别为：5790197，虚拟机上为1099039883.

day13i: 加入哨兵模式，将4个判断减少为2个，模拟器上counter为：6630553.

day14d: 提高分辨率

day14e: 6996091, 虚拟机上为1113289623.

day14g: 修正了keymap.

day16a:任务自动化

day16b:任务休眠

day17a: 增加idle task做哨兵。

day17b: 创建命令后窗口

day18c: tab后多次切换caps和shift会出错。

day18d: 修正day18c错误，这个是因为keytable1中字母大小写的问题导致的。

day18f: 在tools中添加了strcmp的实现.

day18g: 将软盘中33个扇区放在0x60000处。

day19e: type修改，并且代码整理。为使用type，将开头的0x40个扇区放在了0x60000处。添加应用程序，应用程序未使用elf.

day20a: 整理后，dir输出size后有一个'。'.

day20c: makefile添加.map文件生成。

day20d:farjmp改为farcall

day20e：day20d使用farcall,需要自己根据map填写地址，day20e将asm_puts_char注册为中断，不需要调整地址。

day20f: makefile中添加make rerun和make reimage.

day21a1: 从此处开始使用交叉编译。

day21b: 将api.bin编程链接使用 --oformat binary模式，然后固定放在0x8000处。

day21c: 每个api放在单独的文件夹中，api子文件夹下放置makefile, 以方便单独编译。调整了crack点的位置，和自己的root dir保持一致。

day21d: 在asm中.开始的标签名，即便和其他函数中的标签重复，系统也能将他们区分开。

day22b: 添加.lst和.map文件，分析对比数据出错位置。

day22d: 调整kernel中asm文件，在c call 汇编时候，之前是push ebp, 现在去除了，没必要。另外，发现task_a的cursor不闪烁,这是由于cursor_c>=0被错误写成cursor_c>0导致只显示白的，不显示黑色。加了中断号打印，这个对debug是有很用的。