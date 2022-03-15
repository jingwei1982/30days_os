#include "bootpack.h"

void console_task(struct SHEET *sheet, unsigned int memtotal)
{
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	int i, fifobuf[128], *fat = (int *)memman_alloc_4k(memman, 4 * 2880); //因为是int类型，所以分配内存是4*2880. 2880是扇区数目。
	struct CONSOLE cons;
	char cmdline[30];
	cons.sht = sheet;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	*((int *)0x0fec) = (int)&cons;
	// struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x2600);
	// struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;

	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);
	file_readfat(fat, (unsigned char *)(ADR_DISKIMG + 0x000200));

	//显示提示符
	cons_putchar(&cons, '>', 1);
	for (;;)
	{
		io_cli();
		if (fifo32_status(&task->fifo) == 0)
		{
			task_sleep(task);
			io_sti();
		}
		else
		{
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1)
			{
				if (i != 0)
				{
					timer_init(timer, &task->fifo, 0); /* 然后设置0 */
					if (cons.cur_c >= 0)
					{
						cons.cur_c = COL8_FFFFFF;
					}
				}
				else
				{
					timer_init(timer, &task->fifo, 1); /* 然后设置1 */
					if (cons.cur_c >= 0)
					{
						cons.cur_c = COL8_000000;
					}
				}
				timer_settime(timer, 50);
			}
			if (i == 2) // from task a, cursor on
			{
				cons.cur_c = COL8_FFFFFF;
			}
			if (i == 3) // from task a ,cursor off
			{
				boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				cons.cur_c = -1;
			}
			if (256 <= i && i <= 511) // keyboard
			{
				if (i == 8 + 256) // backspace
				{
					if (cons.cur_x > 16)
					{
						//空格擦除光标后前移一位
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				}
				else if (i == 10 + 256) // enter from task a
				{
					// use space to erase the cursor.
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline, &cons, fat, memtotal);
					cons_putchar(&cons, '>', 1);
				}
				else // general char
				{
					if (cons.cur_x < 240)
					{
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			if (cons.cur_c >= 0)
			{
				boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			}
			sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
		}
	}
}

void cons_putchar(struct CONSOLE *cons, int chr, char move)
{
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if (s[0] == 0x09)
	{ /*制表符*/
		for (;;)
		{
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240)
			{
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0)
			{
				break; /*被32整除则break*/
			}
		}
	}
	else if (s[0] == 0x0a)
	{ /*换行*/
		cons_newline(cons);
	}
	else if (s[0] == 0x0d)
	{	/*回车*/
		/*先不做任何操作*/
	}
	else
	{ /*一般字符*/
		putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		if (move != 0)
		{
			/* move为0时光标不后移*/
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240)
			{
				cons_newline(cons);
			}
		}
	}
	return;
}

void cons_putstr0(struct CONSOLE *cons, char *s)
{
	for (; *s != 0; s++)
	{
		cons_putchar(cons, *s, 1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons, char *s, int l)
{
	int i;
	for (i = 0; i < l; i++)
	{
		cons_putchar(cons, s[i], 1);
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	if (cons->cur_y < 28 + 112)
	{
		cons->cur_y += 16; /*到下一行*/
	}
	else
	{
		/*滚动*/
		for (y = 28; y < 28 + 112; y++)
		{
			for (x = 8; x < 8 + 240; x++)
			{
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}
		for (y = 28 + 112; y < 28 + 128; y++)
		{
			for (x = 8; x < 8 + 240; x++)
			{
				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	}
	cons->cur_x = 8;
	return;
}

void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal)
{
	if (strcmp(cmdline, "mem") == 0)
	{
		cmd_mem(cons, memtotal);
	}
	else if (strcmp(cmdline, "cls") == 0)
	{
		cmd_cls(cons);
	}
	else if (strcmp(cmdline, "dir") == 0 || strcmp(cmdline, "ls") == 0)
	{
		cmd_dir(cons);
	}
	else if (strncmp(cmdline, "type ", 5) == 0)
	{
		cmd_type(cons, fat, cmdline);
	}
	else if (cmdline[0] != 0)
	{
		if (cmd_app(cons, fat, cmdline) == 0)
		{
			/*不是命令，也不是空行*/
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}

void cmd_mem(struct CONSOLE *cons, unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	char s[60];
	sprintf(s, "memory %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++)
	{
		for (x = 8; x < 8 + 240; x++)
		{
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x002600);
	int i, j;
	char s[30];
	for (i = 0; i < 224; i++)
	{
		if (finfo[i].name[0] == 0x00)
		{
			break;
		}
		if (finfo[i].name[0] != 0xe5)
		{
			if ((finfo[i].type & 0x18) == 0)
			{
				sprintf(s, "filename.ext %7d\n", finfo[i].size);
				for (j = 0; j < 8; j++)
				{
					s[j] = finfo[i].name[j];
				}
				s[9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO *)(ADR_DISKIMG + 0x002600), 224);
	char *p;
	int i;
	if (finfo != 0)
	{
		/*找到文件的情况*/
		p = (char *)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
		cons_putstr1(cons, p, finfo->size);
		memman_free_4k(memman, (int)p, finfo->size);
	}
	else
	{
		/*没有找到文件的情况*/
		cons_putstr0(cons, "File not found.\n");
	}
	cons_newline(cons);
	return;
}

void cmd_hlt(struct CONSOLE *cons, int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search("HLT.BIN", (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	char *p;
	if (finfo != 0)
	{
		p = (char *)memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x003e00));
		set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER);
		farcall(0, 1003 * 8);
		memman_free_4k(memman, (int)p, finfo->size);
	}
	else
	{
		putfonts8_asc_sht(cons->sht, 8, cons->cur_y, COL8_FFFFFF, COL8_000000, "File not found.", 15);
		cons_newline(cons);
	}
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	char name[18], *p, *q;
	struct TASK *task = task_now();
	int i;

	// genrate the name according to the cmd.
	for (i = 0; i < 13; i++)
	{
		if (cmdline[i] <= ' ')
		{
			break;
		}
		name[i] = cmdline[i];
	}
	name[i] = 0; // set o after the filename.

	// find file
	finfo = file_search(name, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	if (finfo == 0 && name[i - 1] != '.')
	{
		// not find the file, add .bin to research
		name[i] = '.';
		name[i + 1] = 'B';
		name[i + 2] = 'I';
		name[i + 3] = 'N';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *)(ADR_DISKIMG + 0x2600), 224);
	}
	if (finfo != 0)
	{
		// find file.
		// p = (char *)memman_alloc_4k(memman, finfo->size);
		p = (char *)0x8000;
		q = (char *)memman_alloc_4k(memman, 64 * 1024);
		*((int *)0xfe8) = (int)p;
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *)(ADR_DISKIMG + 0x004200 - 0x400)); // 0x4200是数据开始地址，-0x400是fat表有效簇是从3开始。
		set_segmdesc(gdt + 1003, finfo->size - 1, (int)p, AR_CODE32_ER + 0x60);
		set_segmdesc(gdt + 1004, 64 * 1024 - 1, (int)q, AR_DATA32_RW + 0x60);
		start_app(0, 1003 * 8, 64 * 1024, 1004 * 8, &(task->tss.esp0));
		// memman_free_4k(memman, (int)p, finfo->size);
		memman_free_4k(memman, (int)q, 64 * 1024);
		cons_newline(cons);
		return 1;
	}
	// if fail to find file
	return 0;
}

int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax)
{
	int ds_base = *((int *)0xfe8);
	struct TASK *task = task_now();
	struct CONSOLE *cons = (struct CONSOLE *)*((int *)0x0fec);
	struct SHTCTL *shtctl = (struct SHTCTL *)*((int *)0x0fe4);
	struct SHEET *sht;
	int *reg = &eax + 1; // the value behind the eax location.
	char s[12];
	if (edx == 1)
	{
		cons_putchar(cons, eax & 0xff, 1);
		// sprintf(s, "%s\n", eax & 0xff);
		// cons_putstr0(cons, s);
	}
	else if (edx == 2)
	{
		cons_putstr0(cons, (char *)ebx); //移除cs_base
										 // sprintf(s, "%08X\n", ebx);
										 // cons_putstr0(cons, s);
	}
	else if (edx == 3)
	{
		cons_putstr1(cons, (char *)ebx, ecx);
	}
	else if (edx == 4)
	{
		return &(task->tss.esp0);
	}
	else if (edx == 5)
	{
		// edx=5; ebx=win buffer; esi=x size; edi=y size; eax=col_inv; ecx=windows' title.
		// eax is the returned value, eax:sheet handler.
		sht = sheet_alloc(shtctl);
		sheet_setbuf(sht, (char *)ebx + ds_base, esi, edi, eax);
		make_window8((char *)ebx + ds_base, esi, edi, (char *)ecx + ds_base, 0);
		sheet_slide(sht, 100, 50);
		sheet_updown(sht, 3);
		reg[7] = (int)sht;
	}
	else if (edx == 6)
	{
		// edx=6;ebx=windows handler;esi=x; edi=y; eax=color; ecx=string's length;
		// ebp=string
		sht = (struct SHEET *)ebx;
		putfonts8_asc(sht->buf, sht->bxsize, esi, edi, eax, (char *)ebp);
		sheet_refresh(sht, esi, edi, esi + ecx * 8, edi + 16);
	}
	else if (edx == 7)
	{
		// edx=7;ebx=windows handler;eax=x0; ecx=y0; esi=x1; edi=y1;
		// ebp=color
		sht = (struct SHEET *)ebx;
		boxfill8(sht->buf, sht->bxsize, ebp, eax, ecx, esi, edi); // compared with the book, remove the "+ds_base"
		sheet_refresh(sht, eax, ecx, esi + 1, edi + 1);
	}
	else if (edx == 8) 
	{
		memman_init((struct MEMMAN *) (ebx));
		ecx &= 0xfffffff0; /*以16字节为单位*/
		memman_free((struct MEMMAN *) ebx, eax, ecx);
	} 
	else if (edx == 9) 
	{
		ecx = (ecx + 0x0f) & 0xfffffff0; /*以16字节为单位进位取整*/
		reg[7] = memman_alloc((struct MEMMAN *) ebx, ecx);
	} 
	else if (edx == 10) 
	{
		ecx = (ecx + 0x0f) & 0xfffffff0; /*以16字节为单位进位取整*/
		memman_free((struct MEMMAN *) ebx, eax, ecx);
	}
	return 0;
}

int *inthandler0c(int *esp)
{
	struct CONSOLE *cons = (struct CONSOLE *)*((int *)0x0fec);
	struct TASK *task = task_now();
	char s[30];
	cons_putstr0(cons, "\nINT 0C:\n General Protected Exception.\n");
	sprintf(s, "EIP=%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0); // force terminate
}

// esp数组分别为 edi,esi,ebp,ebx,edx,ecx,eax,  （pushad）
// 		 ds, es, (asm_inthandler中push)
// 		 errorno,
// 		 eip,cs, eflags,esp,ss
int *inthandler0d(int *esp)
{
	struct CONSOLE *cons = (struct CONSOLE *)*((int *)0x0fec);
	struct TASK *task = task_now();
	char s[30];
	cons_putstr0(cons, "\nINT 0D:\n General Protected Exception.\n");
	sprintf(s, "EIP=%08X\n", esp[11]);
	cons_putstr0(cons, s);
	return &(task->tss.esp0); // force terminate
}
