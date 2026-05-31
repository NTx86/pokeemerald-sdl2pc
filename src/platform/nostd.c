#include <stddef.h>
#ifdef NO_STD_LIB_ENABLED

//memcpy and memset taken from libgcc
void* memcpy(void *dest, const void *src, size_t len)
{
    char *d = dest;
    const char *s = src;
    while (len--)
        *d++ = *s++;
    return dest;
}

void* memset(void *dest, int val, size_t len)
{
    unsigned char *ptr = dest;
    while (len-- > 0)
        *ptr++ = val;
    return dest;
}

int puts(const char *str)
{
    return 0;
}

int printf_placeholder(const char *format, ...)
{
    return 5;
}

int fprintf_placeholder(void* dummy, const char *format, ...)
{
    return 5;
}

int strcmp (const char* str1, const char* str2)
{
    return 0;
}
#endif

//memset16 and memset32 always build

void* memset16(void *dest, int val, size_t len)
{
    unsigned short *ptr = dest;
    int len16 = len / 2;

    while (len16-- > 0)
        *ptr++ = val;
    return dest;
}

void* memset32(void *dest, int val, size_t len)
{
    unsigned short *ptr = dest;
    int len32 = len / 4;

    while (len32-- > 0)
        *ptr++ = val;
    return dest;
}