#include "bootpack.h"
#include <stdio.h>

void HariMain(void)
{
	struct BOOTINFO *binfo=(struct BOOTINFO *)0x0ff0;;
	char mcursor[256];
	int mx,my;

	init_gdtidt();
	init_pic();
	init_palette();/* 设定调色板 */
	init_screen(binfo->vram,binfo->scrnx,binfo->scrny);

	// char s[40];
	// sprintf(s,"scrnx=%d",binfo->scrnx);
	// putfonts8_asc(binfo->vram,binfo->scrnx,16,64,COL8_FFFFFF,s);

	mx=(binfo->scrnx-16)/2;
	my=(binfo->scrny-28-16)/2;
	init_mouse_cursor8(mcursor,COL8_008484);
	putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mcursor,16);

	for (;;) {
		io_hlt();
	}
}

