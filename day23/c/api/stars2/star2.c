int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
void api_putstrwin(int win, int x, int y, int col, int len, char *str);
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
void api_point(int win, int x, int y, int col);
void api_refreshwin(int win, int x0, int y0, int x1, int y1);
void api_end(void);
void api_putstr0(char *s);
int rand(void);
void srand(unsigned int seed);
char buf[150 * 50];

void HariMain(void)
{
  int win, i, x, y;
  win = api_openwin(buf, 150, 100, -1, "star2");
  api_boxfilwin(win, 6, 26, 143, 93, 0);
  for (i = 0; i < 50; i++)
  {
    x = (rand() % 137) + 6;
    y = (rand() % 67) + 26;
    api_point(win, x, y, 3);
  }
  api_refreshwin(win, 6, 26, 144, 94);
  api_end();
}

static unsigned long int next = 1;

int rand(void) // RAND_MAX assumed to be 32767
{
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed)
{
  next = seed;
}