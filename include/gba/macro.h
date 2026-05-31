#ifndef GUARD_GBA_MACRO_H
#define GUARD_GBA_MACRO_H
#include "platform/nostd.h"

#define CpuFill16(value, dest, size) memset16(dest, value, size)
#define CpuFill32(value, dest, size) memset32(dest, value, size)

#define CpuCopy8(src, dest, size) memcpy(dest, src, size)
#define CpuCopy16(src, dest, size) memcpy(dest, src, size)
#define CpuCopy32(src, dest, size) memcpy(dest, src, size)

#define CpuFastFill16(value, dest, size) memset(dest, value, size)

#define CpuFastFill8(value, dest, size) memset(dest, value, size)

#define CpuFastCopy(src, dest, size) memcpy(dest, src, size)

#define DmaFill16(dmaNum, value, dest, size) CpuFill16(value, dest, size)
#define DmaFill32(dmaNum, value, dest, size) CpuFill32(value, dest, size)

#define DmaClear16(dmaNum, dest, size) DmaFill16(dmaNum, 0, dest, size);
#define DmaClear32(dmaNum, dest, size) DmaFill32(dmaNum, 0, dest, size);

#define DmaCopy16(dmaNum, src, dest, size) CpuCopy16(src, dest, size)
#define DmaCopy32(dmaNum, src, dest, size) CpuCopy32(src, dest, size)

#define DmaCopyLarge(dmaNum, src, dest, size, block, bit) \
{                                                         \
    const void *_src = src;                               \
    void *_dest = dest;                                   \
    u32 _size = size;                                     \
    while (1)                                             \
    {                                                     \
        DmaCopy##bit(dmaNum, _src, _dest, (block));       \
        _src += (block);                                  \
        _dest += (block);                                 \
        _size -= (block);                                 \
        if (_size <= (block))                             \
        {                                                 \
            DmaCopy##bit(dmaNum, _src, _dest, _size);     \
            break;                                        \
        }                                                 \
    }                                                     \
}

#define DmaCopyLarge16(dmaNum, src, dest, size, block) DmaCopyLarge(dmaNum, src, dest, size, block, 16)

#define DmaCopyLarge32(dmaNum, src, dest, size, block) DmaCopyLarge(dmaNum, src, dest, size, block, 32)

#define DmaFillLarge(dmaNum, value, dest, size, block, bit) \
{                                                           \
    void *_dest = dest;                                     \
    u32 _size = size;                                       \
    while (1)                                               \
    {                                                       \
        DmaFill##bit(dmaNum, value, _dest, (block));       \
        _dest += (block);                                   \
        _size -= (block);                                   \
        if (_size <= (block))                               \
        {                                                   \
            DmaFill##bit(dmaNum, value, _dest, _size);     \
            break;                                          \
        }                                                   \
    }                                                       \
}

#define DmaFillLarge16(dmaNum, value, dest, size, block) DmaFillLarge(dmaNum, value, dest, size, block, 16)

#define DmaFillLarge32(dmaNum, value, dest, size, block) DmaFillLarge(dmaNum, value, dest, size, block, 32)

#define DmaClearLarge(dmaNum, dest, size, block, bit) \
{                                                           \
    void *_dest = dest;                                     \
    u32 _size = size;                                       \
    while (1)                                               \
    {                                                       \
        DmaFill##bit(dmaNum, 0, _dest, (block));       \
        _dest += (block);                                   \
        _size -= (block);                                   \
        if (_size <= (block))                               \
        {                                                   \
            DmaFill##bit(dmaNum, 0, _dest, _size);     \
            break;                                          \
        }                                                   \
    }                                                       \
}

#define DmaClearLarge16(dmaNum, dest, size, block) DmaClearLarge(dmaNum, dest, size, block, 16)

#define DmaClearLarge32(dmaNum, dest, size, block) DmaClearLarge(dmaNum, dest, size, block, 32)

#define DmaCopyDefvars(dmaNum, src, dest, size, bit) \
{                                                    \
    const void *_src = src;                          \
    void *_dest = dest;                              \
    u32 _size = size;                                \
    DmaCopy##bit(dmaNum, _src, _dest, _size);        \
}

#define DmaCopy16Defvars(dmaNum, src, dest, size) DmaCopyDefvars(dmaNum, src, dest, size, 16)
#define DmaCopy32Defvars(dmaNum, src, dest, size) DmaCopyDefvars(dmaNum, src, dest, size, 32)

#define DmaFillDefvars(dmaNum, value, dest, size, bit) \
{                                                      \
    void *_dest = dest;                                \
    u32 _size = size;                                  \
    DmaFill##bit(dmaNum, value, _dest, _size);         \
}

#define DmaFill16Defvars(dmaNum, value, dest, size) DmaFillDefvars(dmaNum, value, dest, size, 16)
#define DmaFill32Defvars(dmaNum, value, dest, size) DmaFillDefvars(dmaNum, value, dest, size, 32)

#define DmaClearDefvars(dmaNum, dest, size, bit) \
{                                                \
    void *_dest = dest;                          \
    u32 _size = size;                            \
    DmaClear##bit(dmaNum, _dest, _size);         \
}

#define DmaClear16Defvars(dmaNum, dest, size) DmaClearDefvars(dmaNum, dest, size, 16)
#define DmaClear32Defvars(dmaNum, dest, size) DmaClearDefvars(dmaNum, dest, size, 32)

#define IntrEnable(flags)                                       \
{                                                               \
    u16 imeTemp;                                                \
                                                                \
    imeTemp = REG_IME;                                          \
    REG_IME = 0;                                                \
    REG_IE |= flags;                                            \
    REG_IME = imeTemp;                                          \
}                                                               \

#endif // GUARD_GBA_MACRO_H
