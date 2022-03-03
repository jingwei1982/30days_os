/* bootpack */

#include "bootpack.h"
#include <stdio.h>

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
	char s[40], keybuf[32], mousebuf[128], timerbuf[8];
	int mx, my, i;
	unsigned int memtotal, count = 0;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse, *sht_win;
	unsigned char *buf_back, buf_mouse[256], *buf_win;
	struct FIFO8 timerfifo;
	struct TIMER *timer, *timer2, *timer3;

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC的初始化已经完成，于是开放CPU的中断 */

	fifo8_init(&keyfifo, 32, keybuf);
	fifo8_init(&mousefifo, 128, mousebuf);

	init_pit();

	io_out8(PIC0_IMR, 0xf8); /* 开放PIC1和键盘中断(11111000) */
	io_out8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */

	fifo8_init(&timerfifo, 8, timerbuf);
	timer = timer_alloc();
	timer_init(timer, &timerfifo, 10);
	timer_settime(timer, 1000);

	timer2 = timer_alloc();
	timer_init(timer2, &timerfifo, 3);
	timer_settime(timer2, 300);

	timer3 = timer_alloc();
	timer_init(timer3, &timerfifo, 1);
	timer_settime(timer3, 50);

	init_keyboard();
	enable_mouse(&mdec);
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	// memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	sht_back = sheet_alloc(shtctl);
	sht_mouse = sheet_alloc(shtctl);
	sht_win = sheet_alloc(shtctl);
	buf_back = (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	buf_win = (unsigned char *)memman_alloc_4k(memman, 160 * 52);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1); /* 没有透明色 */
	sheet_setbuf(sht_win, buf_win, 160, 52, -1);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99); /* 透明色号99 */
	init_screen(buf_back, binfo->scrnx, binfo->scrny);
	make_window8(buf_win, 160, 52, "counter");
	init_mouse_cursor8(buf_mouse, 99); /* 背景色号99 */
	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_win, 80, 72);
	mx = (binfo->scrnx - 16) / 2; /* 按显示在画面中央来计算坐标 */
	my = (binfo->scrny - 28 - 16) / 2;
	sheet_slide(sht_mouse, mx, my);
	sheet_updown(sht_back, 0);
	sheet_updown(sht_win, 1);
	sheet_updown(sht_mouse, 2);
	sprintf(s, "(%3d, %3d)", mx, my);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
	sprintf(s, "memory %dMB free : %dKB", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	putfonts8_asc(buf_back, binfo->scrnx, 0, 32, COL8_FFFFFF, s);
	sheet_refresh(sht_back, 0, 0, binfo->scrnx, 48); /* 刷新文字 */

	for (;;)
	{
		count++;
		// sprintf(s, "%10d", timerctl.count);
		// boxfill8(buf_win, 160, COL8_C6C6C6, 40, 28, 119, 43);
		// putfonts8_asc(buf_win, 160, 40, 28, COL8_000000, s);
		// sheet_refresh(sht_win, 40, 28, 120, 44);
		io_cli();
		if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) + fifo8_status(&timerfifo) == 0)
		{
			io_sti();
		}
		else
		{
			if (fifo8_status(&keyfifo) != 0)
			{
				i = fifo8_get(&keyfifo);
				io_sti();
				sprintf(s, "%02X", i);
				boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
				putfonts8_asc(buf_back, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
				sheet_refresh(sht_back, 0, 16, 16, 32); /* 刷新文字 */
			}
			else if (fifo8_status(&mousefifo) != 0)
			{
				i = fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0)
				{
					/* 3字节都凑齐了，所以把它们显示出来*/
					sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
					if ((mdec.btn & 0x01) != 0)
					{
						s[1] = 'L';
					}
					if ((mdec.btn & 0x02) != 0)
					{
						s[3] = 'R';
					}
					if ((mdec.btn & 0x04) != 0)
					{
						s[2] = 'C';
					}
					putfonts8_asc_sht(sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, 40);
					/* 移动光标 */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0)
					{
						mx = 0;
					}
					if (my < 0)
					{
						my = 0;
					}
					if (mx > binfo->scrnx - 1)
					{
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1)
					{
						my = binfo->scrny - 1;
					}
					sprintf(s, "(%3d, %3d)", mx, my);
					putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 20);
				}
			}
			else if (fifo8_status(&timerfifo) != 0)
			{
				i = fifo8_get(&timerfifo); /* 首先读入（为了设定起始点） */
				io_sti();
				if(i==10)
				{
					putfonts8_asc_sht(sht_back,0,64,COL8_FFFFFF,COL8_008484,"10[sec]",7);
					sprintf(s, "%010d", count);
					putfonts8_asc_sht(sht_win,40,28,COL8_000000,COL8_C6C6C6,s,10);

				}
				else if(i==3)
				{
					putfonts8_asc_sht(sht_back,0,80,COL8_FFFFFF,COL8_008484,"3[sec]",7);
					count=0;
				}
				else
				{
					if (i != 0)
					{
						timer_init(timer3, &timerfifo, 0); /* 然后设置0 */
						boxfill8(buf_back, binfo->scrnx, COL8_FFFFFF, 8, 96, 15, 111);
					}
					else
					{
						timer_init(timer3, &timerfifo, 1); /* 然后设置1 */
						boxfill8(buf_back, binfo->scrnx, COL8_008484, 8, 96, 15, 111);
					}
					timer_settime(timer3, 50);
					sheet_refresh(sht_back, 8, 96, 16, 112);
				}	
			}
		}
	}
}