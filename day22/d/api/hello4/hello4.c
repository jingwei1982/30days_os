void api_putchar(int c);
void api_end(void);
void api_putstr0(char *s);

void HariMain(void)
{
    api_putstr0("hello world\n");
    api_end();
}