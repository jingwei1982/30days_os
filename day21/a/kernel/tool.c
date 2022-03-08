#include <stdarg.h>  
#include "bootpack.h"

#define NULL 0
#define ZEROPAD 1
#define SIGN 2
#define PLUS 4
#define SPACE 8
#define LEFT 16
#define SMALL 32
#define SPECIAL 64

//如果字符串中为数字，则返回数字   
static int skip_atoi(const char **s)  
{  
    int i = 0;  
    while (isdigit(**s))  
        i = i * 10 + *((*s)++) - '0';  
    return i;  
}  
  
static inline int isdigit(int ch)  
{  
    return (ch >= '0') && (ch <= '9'); //返回从字符中提取0-9的数字  
}     
  
//这个宏主要用来实现判断是要转化成什么进制数   
#define __do_div(n, base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

static char *number(char *str, long num, int base, int size, int precision,  
            int type)  
{  
      
    static const char digits[16] = "0123456789ABCDEF";   
  
    char tmp[66];  
    char c, sign, locase;  
    int i;  
  
    locase = (type & SMALL);  
    if (type & LEFT)  
        type &= ~ZEROPAD;  
    if (base < 2 || base > 36)  
        return NULL;  
    c = (type & ZEROPAD) ? '0' : ' ';  
    sign = 0;  
    if (type & SIGN) {  
        if (num < 0) {  
            sign = '-';  
            num = -num;  
            size--;  
        } else if (type & PLUS) {  
            sign = '+';  
            size--;  
        } else if (type & SPACE) {  
            sign = ' ';  
            size--;  
        }  
    }  
    //检测进制数，是要2进制还是要8进制还是16进制   
    if (type & SPECIAL) {  
        if (base == 16)  
            size -= 2;  
        else if (base == 8)  
            size--;  
    }  
    i = 0;  
    if (num == 0)  
        tmp[i++] = '0';  
    else  
        while (num != 0)  
            tmp[i++] = (digits[__do_div(num, base)] | locase);  
    if (i > precision)  
        precision = i;  
    size -= precision;  
    if (!(type & (ZEROPAD + LEFT)))  
        while (size-- > 0)  
            *str++ = ' ';  
    if (sign)  
        *str++ = sign;  
    if (type & SPECIAL) {  
        if (base == 8)  
            *str++ = '0';  
        else if (base == 16) {  
            *str++ = '0';  
            *str++ = ('X' | locase);  
        }  
    }  
    if (!(type & LEFT))  
        while (size-- > 0)  
            *str++ = c;  
    while (i < precision--)  
        *str++ = '0';  
    while (i-- > 0)  
        *str++ = tmp[i];  
    while (size-- > 0)  
        *str++ = ' ';  
    return str;  
}  
  
int vsprintf(char *buf, const char *fmt, va_list args)  
{  
    int len;  
    unsigned long num;  
    int i, base;  
    char *str;  
    const char *s;  
    int flags;        
    int field_width;      
    int precision;        
    int qualifier;        
    //这里判断，如果在字符串fmt中不存在%这个符号，那么字符串继续往后遍历   
    for (str = buf; *fmt; ++fmt) {  
        if (*fmt != '%') {  
            *str++ = *fmt;  
            continue;  
        }  
  
        //程序设置标志位   
        flags = 0;  
          repeat:  
        ++fmt;        
        //格式控制   
        switch (*fmt) {  
        case '-':  
            flags |= LEFT;  
            goto repeat;  
        case '+':  
            flags |= PLUS;  
            goto repeat;  
        case ' ':  
            flags |= SPACE;  
            goto repeat;  
        case '#':  
            flags |= SPECIAL;  
            goto repeat;  
        case '0':  
            flags |= ZEROPAD;  
            goto repeat;  
        }  
  
        //获取宽度，这里主要是要实现printf的位宽机制   
        field_width = -1;  
        if (isdigit(*fmt))  
            field_width = skip_atoi(&fmt);  
        else if (*fmt == '*') {  
            ++fmt;  
            field_width = va_arg(args, int);  
            if (field_width < 0) {  
                field_width = -field_width;  
                flags |= LEFT;  
            }  
        }  
  
        precision = -1;  
        if (*fmt == '.') {  
            ++fmt;  
            if (isdigit(*fmt))  
                precision = skip_atoi(&fmt);  
            else if (*fmt == '*') {  
                ++fmt;  
                precision = va_arg(args, int);  
            }  
            if (precision < 0)  
                precision = 0;  
        }  
  
          
        qualifier = -1;  
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {  
            qualifier = *fmt;  
            ++fmt;  
        }  
  
          
        base = 10;  
        //以下主要是要实现printf的格式输出 例如：%d , %c , %u ...等等   
        switch (*fmt) {  
        case 'c':  //以字符形式进行输出   
            if (!(flags & LEFT))  
                while (--field_width > 0)  
                    *str++ = ' ';  
            *str++ = (unsigned char)va_arg(args, int);  
            while (--field_width > 0)  
                *str++ = ' ';  
            continue;  
  
        case 's': //以字符串形式进行输出   
            s = va_arg(args, char *);  
            len =strnlen(s, precision); 
  
            if (!(flags & LEFT))  
                while (len < field_width--)  
                    *str++ = ' ';  
            for (i = 0; i < len; ++i)  
                *str++ = *s++;  
            while (len < field_width--)  
                *str++ = ' ';  
            continue;  
  
        case 'p': //以地址形式输出，也就是以16进制数输出   
            if (field_width == -1) {  
                field_width = 2 * sizeof(void *);  
                flags |= ZEROPAD;  
            }  
            str = number(str,  
                     (unsigned long)va_arg(args, void *), 16,  
                     field_width, precision, flags);  
            continue;  
  
        case 'n':  
            if (qualifier == 'l') {  
                long *ip = va_arg(args, long *);  
                *ip = (str - buf);  
            } else {  
                int *ip = va_arg(args, int *);  
                *ip = (str - buf);  
            }  
            continue;  
  
        case '%':     //这里表示字符串中存在%号这个字符   
            *str++ = '%';  
            continue;  
  
              
        case 'o':     //%o 表示8进制输出   
            base = 8;  
            break;  
  
        case 'x':     //%x或者%X 表示16进制输出   
            flags |= SMALL;  
        case 'X':  
            base = 16;  
            break;  
  
        case 'd':     //%d %i整形数输出，%u无符号整形   
        case 'i':  
            flags |= SIGN;  
        case 'u':  
            break;  
  
        default:  
            *str++ = '%';  
            if (*fmt)  
                *str++ = *fmt;  
            else  
                --fmt;  
            continue;  
        }  
        if (qualifier == 'l') //以无符号长整型输出   
            num = va_arg(args, unsigned long);  
        else if (qualifier == 'h') {  
            num = (unsigned short)va_arg(args, int);  
            if (flags & SIGN)  
                num = (short)num;  
        } else if (flags & SIGN)  
            num = va_arg(args, int);  
        else  
            num = va_arg(args, unsigned int);  
        str = number(str, num, base, field_width, precision, flags);  
    }  
    *str = '\0';   //字符串遍历到有\0的地方就停止   
    return str - buf;  
}  
//可变参形式实现sprintf   
int sprintf(char *buf, const char *fmt, ...)  
{  
    va_list args;  
    int i;  
    va_start(args, fmt);  
    //将获取到的fmt格式字符串写入到buf这个缓存里去   
    i = vsprintf(buf, fmt, args);  
    //释放args   
    va_end(args);  
    return i;  
}  
  
//可变参形式进行实现myprintf   
// int myprintf(const char *fmt, ...)  
// {  
//     char printf_buf[1024];  
//     va_list args;  
//     int printed;  
//     va_start(args, fmt);  
//     printed = vsprintf(printf_buf, fmt, args);  
//     va_end(args);  
//     puts(printf_buf);  
//     return printed;  
// }  

int strcmp(const char *str1,const char *str2)
{
    while(1)
    {
        if(*str1=='\0' && *str2=='\0')
            {
                return 0;
            }
        else if((int)*str1>(int)*str2)
            {
                return 1;
            }
        else if((int)*str1<(int)*str2)
            {
                return -1;
            }
        else
            {
                str1++;
                str2++;
            }
    }
}

int strncmp(const char *str1,const char *str2,int count)
{
    int i;
    for (i = 0; i < count;i++)
    {
        if((int)*str1>(int)*str2)
        {
            return 1;
        }
        else if((int)*str1<(int)*str2)
        {
            return -1;
        }
        else
        {
            str1++;
            str2++;
        }
    }
    return 0;
}