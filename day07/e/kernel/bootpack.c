/* bootpack */

#include "bootpack.h"
#include <stdio.h>

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40], mcursor[256],keybuf[32];
	unsigned char i;
	int j,mx, my;

	init_gdtidt();
	init_pic();
	fifo8_init(&keyfifo,32,keybuf);
	io_sti(); /* IDT/PIC的初始化已经完成，于是开放CPU的中断 */

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	mx = (binfo->scrnx - 16) / 2; /* 计算画面的中心坐标*/
	my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

	io_out8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
	io_out8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */


	for (;;) {
		io_cli();
		if(fifo8_status(&keyfifo)==0)
		{
			io_stihlt();	
		}
		else{
			i=fifo8_get(&keyfifo);
			io_sti();
			sprintf(s,"INT Keyboard value:%02X",i);
			boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16,320,31);
			putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16,COL8_FFFFFF,s);
		}
	}
}