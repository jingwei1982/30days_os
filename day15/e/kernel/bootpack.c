/* bootpack */

#include "bootpack.h"
#include <stdio.h>

void task_b_main(struct SHEET *sht_back);

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
	char s[40];
	int fifobuf[128];
	struct FIFO32 fifo;
	int mx, my, i, cursor_x=8,cursor_c=COL8_FFFFFF,task_b_esp;
	unsigned int memtotal, count = 0;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct SHTCTL *shtctl;
	struct SHEET *sht_back, *sht_mouse, *sht_win;
	unsigned char *buf_back, buf_mouse[256], *buf_win;
	struct TIMER *timer, *timer2, *timer3,*timer_ts;
	
	static char keytable[0x54] = {
		0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
		'2', '3', '0', '.'};

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC的初始化已经完成，于是开放CPU的中断 */

	fifo32_init(&fifo, 128, fifobuf);
	init_keyboard(&fifo,256);
	enable_mouse(&fifo,512,&mdec);

	init_pit();

	io_out8(PIC0_IMR, 0xf8); /* 开放PIC1和键盘中断(11111000) */
	io_out8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */

	timer = timer_alloc();
	timer_init(timer, &fifo, 10);
	timer_settime(timer, 1000);

	timer2 = timer_alloc();
	timer_init(timer2, &fifo, 3);
	timer_settime(timer2, 300);

	timer3 = timer_alloc();
	timer_init(timer3, &fifo, 1);
	timer_settime(timer3, 50);

	timer_ts = timer_alloc();
	timer_init(timer_ts, &fifo, 2);
	timer_settime(timer_ts, 2);

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
	make_window8(buf_win, 160, 52, "window");
	make_textbox8(sht_win, 8, 28, 144, 16, COL8_FFFFFF);
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

	tss_a.ldtr = 0;
	tss_a.iomap = 0x40000000;
	tss_b.ldtr = 0;
	tss_b.iomap = 0x40000000;

	load_tr(3 * 8);

	task_b_esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 8;
	*((int *)(task_b_esp+4)) = (int)sht_back;
	tss_b.eip = (int) &task_b_main;
	tss_b.eflags = 0x00000202; /* IF = 1; */
	tss_b.eax = 0;
	tss_b.ecx = 0;
	tss_b.edx = 0;
	tss_b.ebx = 0;
	tss_b.esp = task_b_esp;
	tss_b.ebp = 0;
	tss_b.esi = 0;
	tss_b.edi = 0;
	tss_b.es = 1 * 8;
	tss_b.cs = 2 * 8;
	tss_b.ss = 1 * 8;
	tss_b.ds = 1 * 8;
	tss_b.fs = 1 * 8;
	tss_b.gs = 1 * 8;

	for (;;)
	{
		io_cli();
		if (fifo32_status(&fifo) == 0)
		{
			io_stihlt();
		}
		else
		{
			i = fifo32_get(&fifo);
			io_sti();
			if(i==2)
			{
				farjmp(0, 4 * 8);
				timer_settime(timer_ts, 2);
			}
			else if (256<=i&&i<=511)	//keyboard
			{
				sprintf(s, "%02X", i - 256);
				putfonts8_asc_sht(sht_back, 0, 16, COL8_FFFFFF, COL8_008484, s, 2);
				if (i < 0x54 + 256) 
				{
					if (keytable[i - 256] != 0 && cursor_x<144) 
					{ //一般字符
						s[0] = keytable[i - 256];
						s[1] = 0;
						putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_C6C6C6, s, 1);
						cursor_x += 8;
					}
					if(i==256+0xe&&cursor_x>8)	//退格键
					{
						putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, "", 1);
						cursor_x -= 8;
					}
					boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
					sheet_refresh(sht_win, cursor_x, 28, cursor_x + 8, 44);
				}
			}
			else if (512<=i&&i<=767)		//mouse
			{
				if (mouse_decode(&mdec, i-512) != 0)
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
					putfonts8_asc_sht(sht_back, 32, 16, COL8_FFFFFF, COL8_008484, s, 15);
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
					putfonts8_asc_sht(sht_back, 0, 0, COL8_FFFFFF, COL8_008484, s, 10);
					if((mdec.btn&0x01)!=0)
					{
						sheet_slide(sht_win,mx-80,my-8);
					}
					
				}
			}
			else if(i==10)
			{
				putfonts8_asc_sht(sht_back,0,64,COL8_FFFFFF,COL8_008484,"10[sec]",7);
				farjmp(0,4*8);
			}
			else if(i==3)
			{
				putfonts8_asc_sht(sht_back,0,80,COL8_FFFFFF,COL8_008484,"3[sec]",7);
			}
			else
			{
				if (i != 0)
				{
					timer_init(timer3, &fifo, 0); /* 然后设置0 */
					cursor_c = COL8_000000;
				}
				else
				{
					timer_init(timer3, &fifo, 1); /* 然后设置1 */
					cursor_c = COL8_FFFFFF;
				}
				timer_settime(timer3, 50);
				boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x,28,cursor_x+7, 43);
				sheet_refresh(sht_win, cursor_x, 28, cursor_x+8,44);
			}	
		}
	}
}

void task_b_main(struct SHEET *sht_back)
{
	struct FIFO32 fifo;
	struct TIMER *timer_ts,*timer_put;
	int i, fifobuf[128],count=0;
	char s[11];

	fifo32_init(&fifo, 128, fifobuf);
	timer_ts = timer_alloc();
	timer_init(timer_ts, &fifo, 2);
	timer_settime(timer_ts, 2);

	timer_put = timer_alloc();
	timer_init(timer_put, &fifo, 1);
	timer_settime(timer_put, 1);

	for (;;)
	{
		count++;

		io_cli();
		if(fifo32_status(&fifo)==0)
		{
			io_stihlt();
		}
		else
		{
			i = fifo32_get(&fifo);
			io_sti();
			if(i==1)
			{
				sprintf(s, "%10d", count);
				putfonts8_asc_sht(sht_back,0,144,COL8_FFFFFF,COL8_008484,s,10);
				timer_settime(timer_put, 1);
			}
			else if(i==2)
			{
				farjmp(0, 3 * 8);
				timer_settime(timer_ts, 2);
			}
		}
	}
}