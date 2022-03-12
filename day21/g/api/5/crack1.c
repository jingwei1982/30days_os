void api_end(void);

void HariMain(void)
{
    *((char *)0x0062600) = 0;
    api_end();
}