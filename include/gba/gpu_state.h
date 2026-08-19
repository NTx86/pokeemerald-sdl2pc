#ifndef GUARD_GBA_GPUSTATE_H
#define GUARD_GBA_GPUSTATE_H

#include "gba/types.h"
#include "gba/defines.h"

#define MAX_OAM_SPRITES 128

#define MAX_OAM_MATRICES 32

#define NUM_CHAR_BLOCKS 4

#define NUM_SCREEN_BLOCKS 32

enum
{
    GPU_STATE_DISPCNT,
    GPU_STATE_DISPSTAT,
    GPU_STATE_VCOUNT,
    GPU_STATE_MOSAIC,
    GPU_STATE_BLDCNT,
    GPU_STATE_BLDALPHA,
    GPU_STATE_BLDY
};

enum
{
    GPU_SCANLINE_EFFECT_OFF,
    GPU_SCANLINE_EFFECT_BGX,
    GPU_SCANLINE_EFFECT_BGY,
    GPU_SCANLINE_EFFECT_BGPRIO,
    GPU_SCANLINE_EFFECT_WINDOWX,
    GPU_SCANLINE_EFFECT_WINDOWX_TWOWIN,
    GPU_SCANLINE_EFFECT_WINDOWY,
    GPU_SCANLINE_EFFECT_BLENDCNT,
    GPU_SCANLINE_EFFECT_BLENDALPHA
};

struct GpuScanlineEffect
{
    u8 type;
    u8 param;
    u32 *src;
    u32 position;
};

struct GpuAffineBgState
{
    u8 enabled:1;
    s16 pa, pb, pc, pd;
};

struct GpuBgState
{
    s32 x, y;
    u16 priority:2;
    u16 charBaseBlock;
    u16 screenBaseBlock;
    u16 screenWidth;
    u16 screenHeight;
    u8 mosaic:1;
    u8 palettes:1;
    u8 gbaMode:1;
    u8 hidden:1;
    u8 areaOverflowMode:1;
    struct GpuAffineBgState affine;
};

struct GpuWindowState
{
    u32 x, y;
};

struct GpuMatrix
{
    s16 a;
    s16 b;
    s16 c;
    s16 d;
};

struct GpuState
{
    u8 *gfxData;
    u8 *tileMaps;
    u8 *spriteGfxData;

    struct OamData spriteList[MAX_OAM_SPRITES];
    struct GpuMatrix spriteMatrices[MAX_OAM_SPRITES];

    unsigned char palette[PLTT_SIZE];

    size_t gfxDataSize;
    size_t tileMapsSize;
    size_t spriteGfxDataSize;

    u32 displayControl;
    u32 displayStatus;
    s16 vCount;

    struct GpuBgState bg[NUM_BACKGROUNDS];

    struct {
        struct GpuWindowState state[2];
        u16 in, out;
    } window;

    u16 mosaic;
    u16 blendControl;
    u16 blendAlpha;
    u16 blendCoeff;

    struct GpuScanlineEffect scanlineEffect;
};

extern struct GpuState gpu;

#endif
