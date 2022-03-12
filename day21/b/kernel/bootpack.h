/* asmhead.nas */
struct BOOTINFO
{				/* 0x0ff0-0x0fff */
	char cyls;	/* 启动区读磁盘读到此为止 */
	char leds;	/* 启动时键盘的LED的状态 */
	char vmode; /* 显卡模式为多少位彩色 */
	char reserve;
	short scrnx, scrny; /* 画面分辨率 */
	char *vram;
};
#define ADR_BOOTINFO 0x00000ff0

/* naskfunc.nas */
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
void io_out8(int port, int data);
unsigned char io_in8(int port);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
void load_tr(int tr);
void farjmp(int eip, int cs);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
void asm_cons_putchar(void);
void asm_hrb_api(void);
unsigned int memtest(unsigned int start, unsigned int end);
void farcall(int eip, int cs);

/* graphic.c */

#define COL8_000000 0  //黑
#define COL8_FF0000 1  //亮红
#define COL8_00FF00 2  //亮绿
#define COL8_FFFF00 3  //亮黄
#define COL8_0000FF 4  //亮蓝
#define COL8_FF00FF 5  //亮紫
#define COL8_00FFFF 6  //浅亮蓝
#define COL8_FFFFFF 7  //白
#define COL8_C6C6C6 8  //亮灰
#define COL8_840000 9  //暗红
#define COL8_008400 10 //暗绿
#define COL8_848400 11 //暗黄
#define COL8_000084 12 //暗蓝
#define COL8_840084 13 //暗紫
#define COL8_008484 14 //浅暗蓝
#define COL8_848484 15 //暗灰

void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
				 int pysize, int px0, int py0, char *buf, int bxsize);
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);

/* dsctbl.c */
struct SEGMENT_DESCRIPTOR
{
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR
{
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#define ADR_IDT 0x0026f800
#define LIMIT_IDT 0x000007ff
#define ADR_GDT 0x00270000
#define LIMIT_GDT 0x0000ffff
#define ADR_BOTPAK 0x00280000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_TSS32 0x0089
#define AR_INTGATE32 0x008e

/* int.c */
#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

void init_pic(void);
void inthandler21(int *esp);
void inthandler27(int *esp);
void inthandler2c(int *esp);
#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

/*fifo.c*/
struct FIFO32
{
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};

struct FIFO32 *mousefifo, *keyfifo;

void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

/* tool.c*/
#include <stdarg.h>
#include <stddef.h>
// #include <string.h>

static int skip_atoi(const char **s);
static inline int isdigit(int ch);
static char *number(char *str, long num, int base, int size, int precision,
					int type);
int vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, int count);
size_t strlen(const char *str);
size_t strnlen(const char *str, size_t maxsize);

/*mouse.c*/
struct MOUSE_DEC
{
	unsigned char buf[3], phase;
	int x, y, btn;
};

void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

/*keyboard.c*/
#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4
#define KEYCMD_LED 0xed

void init_keyboard(struct FIFO32 *fifo, int data0);
void wait_KBC_sendready(void);

/*memmory.c*/

#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000

#define MEMMAN_FREES 4090 /* 大约是32KB*/

struct FREEINFO
{ /* 可用信息 */
	unsigned int addr, size;
};
struct MEMMAN
{ /* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};

void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

#define MEMMAN_ADDR 0x003c0000
#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000

/* sheet.c */
#define MAX_SHEETS 256
#define SHEET_USE 1

struct SHEET
{
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
};

/* 	buf ：记录图层上所描内容的地址
	bxsize*bysize: 图层大小
	vx0/vy0: 图层坐标
	col_inv: 透明色号 （coler & invisible)
	height:  图层高度
	flags:  设定信息
	*/

struct SHTCTL
{
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};

/*	vram/xsize/ysize: vram的地址和画面的尺寸
	top： 最上面图层的高度
	sheets0中的图层顺序混乱，按照高度升序排列后，将地址写入sheets中。
*/

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1);
void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/*timer.c*/

#define MAX_TIMER 500
struct TIMER
{
	struct TIMER *next_timer;
	unsigned int timeout, flags;
	struct FIFO32 *fifo;
	unsigned char data;
};

struct TIMERCTL
{
	unsigned int count, next_time; // next指下一个中断点，using指的是使用数目。
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
struct TIMERCTL timerctl;

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040
#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2

void init_pit(void);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, unsigned char data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
void inthandler20(int *esp);

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);

// mtask.c
#define MAX_TASKS 1000
#define MAX_TASKS_LV 100
#define MAX_TASKLEVELS 10
#define TASK_GDT0 3 //定义从GDT的几号开始分配给TSS

struct TSS32
{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

struct TASK
{
	int sel, flags; // sel存放GDT编号
	int level, priority;
	struct FIFO32 fifo;
	struct TSS32 tss;
};

struct TASKLEVEL
{
	int running; //正在运行的任务数量
	int now;	 //记录正在运行的是哪个任务
	struct TASK *tasks[MAX_TASKS_LV];
};

struct TASKCTL
{
	int now_lv;		//现在活动的level
	char lv_change; //下次任务切换时候是否需要改变level
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};

struct TASKCTL *taskctl;
struct TIMER *task_timer;

struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);
struct TASK *task_now(void);
void task_add(struct TASK *task);
void task_remove(struct TASK *task);
void task_switchsub(void);
void task_idle(void);

// file
#define ADR_DISKIMG 0x60000
struct FILEINFO
{
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
void file_readfat(int *fat, unsigned char *img);
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);

//*console.c
struct CONSOLE
{
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
};

void console_task(struct SHEET *sheet, unsigned int memtotal);
void cons_putchar(struct CONSOLE *cons, int chr, char move);
void cons_putstr0(struct CONSOLE *cons, char *s);
void cons_putstr1(struct CONSOLE *cons, char *s, int l);
void cons_newline(struct CONSOLE *cons);
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);
void cmd_hlt(struct CONSOLE *cons, int *fat);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);
void hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);