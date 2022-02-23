/* bootpack */

#include "bootpack.h"
#include <stdio.h>

extern struct FIFO8 keyfifo,mousefifo;

void HariMain(void)
{
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	char s[40], mcursor[256],keybuf[32],mousebuf[128];
	unsigned char mouse_dbuf[3],mouse_phase;
	int i,j,mx, my;
	struct MOUSE_DEC mdec;

	init_gdtidt();
	init_pic();
	
	io_sti(); /* IDT/PIC的初始化已经完成，于是开放CPU的中断 */

	fifo8_init(&keyfifo,32,keybuf);
	fifo8_init(&mousefifo,128,mousebuf);

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
	init_keyboard();
	enable_mouse(&mdec);

	for (;;) {
		io_cli();
		if(fifo8_status(&keyfifo)+fifo8_status(&mousefifo)==0)
		{
			io_stihlt();	
		}
		else{
			if(fifo8_status(&keyfifo)!=0){
				i=fifo8_get(&keyfifo);
				io_sti();
				sprintf(s,"INT Keyboard value:%02X",i);
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16,320,31);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16,COL8_FFFFFF,s);
			}
			else if(fifo8_status(&mousefifo)!=0){
				i=fifo8_get(&mousefifo);
				io_sti();
				if (mouse_decode(&mdec, i) != 0) {
					/* 3字节都凑齐了，所以把它们显示出来*/
				// sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
				// if ((mdec.btn & 0x01) != 0) {
				// 	s[1] = 'L';
				// }
				// if ((mdec.btn & 0x02) != 0) {
				// 	s[3] = 'R';
				// }
				// if ((mdec.btn & 0x04) != 0) {
				// 	s[2] = 'C';
				// }
	
				sprintf(s,"INT mouse value:%02X %02X %02X",mdec.buf[0],mdec.buf[1],mdec.buf[2]);
				boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 32,319,47);
				putfonts8_asc(binfo->vram, binfo->scrnx, 0, 32,COL8_FFFFFF,s);
				}
			}
		}
	}
}


void wait_KBC_sendready(void)
{
	/* 等待键盘控制电路准备完毕 */
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

void init_keyboard(void)
{
	/* 初始化键盘控制电路 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}

#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

void enable_mouse(struct MOUSE_DEC *mdec){
	/* 鼠标有效 */
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	/* 顺利的话，ACK(0xfa)会被送过来 */
	mdec->phase = 0; /* 等待0xfa的阶段 */    
	return;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat){
	if (mdec->phase == 0) {
		/* 等待鼠标的0xfa的阶段 */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}        
		return 0;
	}
	if (mdec->phase == 1) {
		/* 等待鼠标第一字节的阶段 */
		mdec->buf[0] = dat;
		mdec->phase = 2;
		return 0;
	}
	if (mdec->phase == 2) {
		/* 等待鼠标第二字节的阶段 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* 等待鼠标第二字节的阶段 */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}     
		/* 鼠标的y方向与画面符号相反 */   
		mdec->y = - mdec->y; 
		return 1;
	}
	/* 应该不可能到这里来 */
	return -1; 
}