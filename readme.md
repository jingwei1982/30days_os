platfrom: ubuntu 20.04 LTS

Tools: nasm, gcc, dd, bochs

Q: 如何运行？

A: 在ubuntu环境中，进入目录后，执行make run可以看到在虚拟机中运行的效果。

Q:如何查看bin文件？

A: xxd -a -u -g 1 xxx.bin或者hexdump -C xxx.bin



相对课本，目录基本和教材保持已知，主要改动如下：

day02a: 增加清屏和设置cursor。

day03a: display改成函数调用，添加读一个扇区的功能.已知问题，打印比较乱，回车后cusor没有回到最左边。

day03b: 教材上是固定写haribote.nas地址，然后读入固定的柱面，有很多局限性，现在ipl.asm中增加搜索haribote.bin的代码，会用到FAT和根目录搜索。碰到的问题，将haribote.bin放到0x820:0000会出错，但放在0x1000:0000却是正常的。

day03g: 进入haribote，打印了一个L验证正确进入了haribote。

day03g1: make run后会显示"SLDT: not recognized in real or virtual-8086 mode", 改变的是添加了[section .text]。

day03h: 和教材保持一致，但是去掉了org 0xc200,这是因为day03b的改动引起的。

day03j:加入了汇编和C混合编程，编译为elf格式，通过loader将kernel加载进内存，并跳转到Ttext入口。

day04a: 汇编和C互相调用，可以正常显示纯白屏幕。

day04f: 可以显示窗口。




