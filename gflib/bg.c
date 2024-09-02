#include <limits.h>
#include "global.h"
#include "bg.h"
#include "dma3.h"
#include "gpu_regs.h"
#include "gpu_main.h"

#define DISPCNT_ALL_BG_AND_MODE_BITS    (DISPCNT_BG_ALL_ON | 0x7)

struct BgControl
{
    struct BgConfig {
        u8 visible:1;
        u8 unknown_1:1;
        u16 screenWidth;
        u16 screenHeight;
        u8 priority:2;
        u8 mosaic:1;
        u8 wraparound:1;

        u8 charBaseIndex;
        u8 mapBaseIndex;
        u8 paletteMode:1;
        u8 gbaMode:1;
    } configs[NUM_BACKGROUNDS];

    u16 bgVisibilityAndMode;
};

struct BgConfig2
{
    u32 baseTile:10;
    u32 basePalette:4;

    void *tilemap;
    s32 bg_x;
    s32 bg_y;
};

static struct BgControl sGpuBgConfigs;
static struct BgConfig2 sGpuBgConfigs2[NUM_BACKGROUNDS];

u32 gWindowTileAutoAllocEnabled;

static const struct BgConfig sZeroedBgControlStruct = { 0 };

static u32 GetBgType(u8 bg);

void ResetBgs(void)
{
    ResetBgControlStructs();
    sGpuBgConfigs.bgVisibilityAndMode = 0;
    SetTextModeAndHideBgs();
}

static void SetBgModeInternal(u8 bgMode)
{
    sGpuBgConfigs.bgVisibilityAndMode &= ~0x7;
    sGpuBgConfigs.bgVisibilityAndMode |= bgMode;
}

u8 GetBgMode(void)
{
    return sGpuBgConfigs.bgVisibilityAndMode & 0x7;
}

void ResetBgControlStructs(void)
{
    int i;

    for (i = 0; i < NUM_BACKGROUNDS; i++)
    {
        sGpuBgConfigs.configs[i] = sZeroedBgControlStruct;
    }
}

void Unused_ResetBgControlStruct(u8 bg)
{
    if (!IsInvalidBg(bg))
    {
        sGpuBgConfigs.configs[bg] = sZeroedBgControlStruct;
    }
}

enum
{
    BG_CTRL_ATTR_VISIBLE = 1,
    BG_CTRL_ATTR_CHARBASEINDEX = 2,
    BG_CTRL_ATTR_MAPBASEINDEX = 3,
    BG_CTRL_ATTR_SCREENWIDTH = 4,
    BG_CTRL_ATTR_SCREENHEIGHT = 5,
    BG_CTRL_ATTR_PALETTEMODE = 6,
    BG_CTRL_ATTR_GBAMODE = 7,
    BG_CTRL_ATTR_PRIORITY = 8,
    BG_CTRL_ATTR_MOSAIC = 9,
    BG_CTRL_ATTR_WRAPAROUND = 10,
};

static void SetBgControlAttributes(u8 bg, u16 charBaseIndex, u16 mapBaseIndex, u16 screenWidth, u16 screenHeight, u16 paletteMode, u16 gbaMode, u16 priority, u16 mosaic, u16 wraparound)
{
    if (!IsInvalidBg(bg))
    {
        if (charBaseIndex != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].charBaseIndex = charBaseIndex;
        }

        if (mapBaseIndex != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].mapBaseIndex = mapBaseIndex;
        }

        if (screenWidth != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].screenWidth = screenWidth;
        }

        if (screenHeight != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].screenHeight = screenHeight;
        }

        if (paletteMode != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].paletteMode = paletteMode;
        }

        if (gbaMode != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].gbaMode = gbaMode;
        }

        if (priority != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].priority = priority;
        }

        if (mosaic != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].mosaic = mosaic;
        }

        if (wraparound != 0xFFFF)
        {
            sGpuBgConfigs.configs[bg].wraparound = wraparound;
        }

        sGpuBgConfigs.configs[bg].visible = 1;
    }
}

static u16 GetBgControlAttribute(u8 bg, u8 attributeId)
{
    if (!IsInvalidBg(bg) && sGpuBgConfigs.configs[bg].visible)
    {
        switch (attributeId)
        {
        case BG_CTRL_ATTR_VISIBLE:
            return sGpuBgConfigs.configs[bg].visible;
        case BG_CTRL_ATTR_CHARBASEINDEX:
            return sGpuBgConfigs.configs[bg].charBaseIndex;
        case BG_CTRL_ATTR_MAPBASEINDEX:
            return sGpuBgConfigs.configs[bg].mapBaseIndex;
        case BG_CTRL_ATTR_SCREENWIDTH:
            return sGpuBgConfigs.configs[bg].screenWidth;
        case BG_CTRL_ATTR_SCREENHEIGHT:
            return sGpuBgConfigs.configs[bg].screenHeight;
        case BG_CTRL_ATTR_PALETTEMODE:
            return sGpuBgConfigs.configs[bg].paletteMode;
        case BG_CTRL_ATTR_GBAMODE:
            return sGpuBgConfigs.configs[bg].gbaMode;
        case BG_CTRL_ATTR_PRIORITY:
            return sGpuBgConfigs.configs[bg].priority;
        case BG_CTRL_ATTR_MOSAIC:
            return sGpuBgConfigs.configs[bg].mosaic;
        case BG_CTRL_ATTR_WRAPAROUND:
            return sGpuBgConfigs.configs[bg].wraparound;
        }
    }

    return 0xFF;
}

u8 LoadBgVram(u8 bg, const void *src, size_t size, size_t destOffset, u8 mode)
{
    size_t offset;
    void *ptr;
    void *start, *end;

    if (IsInvalidBg(bg) || !sGpuBgConfigs.configs[bg].visible)
        return -1;

    switch (mode)
    {
    case 0x1:
        if (size > BG_CHAR_SIZE)
            size = BG_CHAR_SIZE;
            // abort();
        offset = sGpuBgConfigs.configs[bg].charBaseIndex * BG_CHAR_SIZE;
        start = gpu.gfxData;
        end = start + gpu.gfxDataSize;
        break;
    case 0x2:
        if (size > BG_SCREEN_SIZE)
            size = BG_SCREEN_SIZE;
            // abort();
        offset = sGpuBgConfigs.configs[bg].mapBaseIndex * BG_SCREEN_SIZE;
        start = gpu.tileMaps;
        end = start + gpu.tileMapsSize;
        break;
    default:
        return -1;
    }

    ptr = start + (offset + destOffset);

    if (ptr >= end || (ptr + size) >= end)
        abort();

    return RequestDma3Copy(src, ptr, size, 0);
}

static void ShowBgInternal(u8 bg)
{
    if (!IsInvalidBg(bg) && sGpuBgConfigs.configs[bg].visible)
    {
        SetGpuBackgroundPriority(bg, sGpuBgConfigs.configs[bg].priority);
        SetGpuBackgroundCharBaseBlock(bg, sGpuBgConfigs.configs[bg].charBaseIndex);
        SetGpuBackgroundMosaicEnabled(bg, sGpuBgConfigs.configs[bg].mosaic);
        SetGpuBackground8bppMode(bg, sGpuBgConfigs.configs[bg].paletteMode);
        SetGpuBackgroundGbaMode(bg, sGpuBgConfigs.configs[bg].gbaMode);
        SetGpuBackgroundScreenBaseBlock(bg, sGpuBgConfigs.configs[bg].mapBaseIndex);
        SetGpuBackgroundAreaOverflowMode(bg, sGpuBgConfigs.configs[bg].wraparound);
        SetGpuBackgroundWidth(bg, sGpuBgConfigs.configs[bg].screenWidth);
        SetGpuBackgroundHeight(bg, sGpuBgConfigs.configs[bg].screenHeight);

        sGpuBgConfigs.bgVisibilityAndMode |= 1 << (bg + 8);
        sGpuBgConfigs.bgVisibilityAndMode &= DISPCNT_ALL_BG_AND_MODE_BITS;
    }
}

static void HideBgInternal(u8 bg)
{
    if (!IsInvalidBg(bg))
    {
        sGpuBgConfigs.bgVisibilityAndMode &= ~(1 << (bg + 8));
        sGpuBgConfigs.bgVisibilityAndMode &= DISPCNT_ALL_BG_AND_MODE_BITS;
    }
}

static void SyncBgVisibilityAndMode(void)
{
    SetGpuState(GPU_STATE_DISPCNT, (GetGpuState(GPU_STATE_DISPCNT) & ~DISPCNT_ALL_BG_AND_MODE_BITS) | sGpuBgConfigs.bgVisibilityAndMode);
}

void SetTextModeAndHideBgs(void)
{
    SetGpuState(GPU_STATE_DISPCNT, GetGpuState(GPU_STATE_DISPCNT) & ~DISPCNT_ALL_BG_AND_MODE_BITS);
}

static void SetBgAffineInternal(u8 bg, s32 srcCenterX, s32 srcCenterY, s16 dispCenterX, s16 dispCenterY, s16 scaleX, s16 scaleY, u16 rotationAngle)
{
    struct BgAffineSrcData src;
    struct BgAffineDstData dest;

    switch (sGpuBgConfigs.bgVisibilityAndMode & 0x7)
    {
    default:
    case 0:
        return;
    case 1:
        if (bg != 2)
            return;
        break;
    case 2:
        if (bg != 2 && bg != 3)
            return;
        break;
    }

    src.texX = srcCenterX;
    src.texY = srcCenterY;
    src.scrX = dispCenterX;
    src.scrY = dispCenterY;
    src.sx = scaleX;
    src.sy = scaleY;
    src.alpha = rotationAngle;

    BgAffineSet(&src, &dest, 1);

    SetGpuAffineBgA(2, dest.pa);
    SetGpuAffineBgB(2, dest.pb);
    SetGpuAffineBgC(2, dest.pc);
    SetGpuAffineBgD(2, dest.pd);
    SetGpuBackgroundX(2, dest.dx);
    SetGpuBackgroundY(2, dest.dy);
}

bool8 IsInvalidBg(u8 bg)
{
    if (bg >= NUM_BACKGROUNDS)
        return TRUE;
    else
        return FALSE;
}

// From FRLG. Dummied out.
int BgTileAllocOp(int bg, int offset, int count, int mode)
{
    return 0;
}

void ResetBgsAndClearDma3BusyFlags(u32 leftoverFireRedLeafGreenVariable)
{
    ResetBgs();

    gWindowTileAutoAllocEnabled = leftoverFireRedLeafGreenVariable;
}

void InitBgsFromTemplates(u8 bgMode, const struct BgTemplate *templates, u8 numTemplates)
{
    int i;
    u8 bg;

    SetBgModeInternal(bgMode);
    ResetBgControlStructs();

    for (i = 0; i < numTemplates; i++)
    {
        bg = templates[i].bg;
        if (bg < NUM_BACKGROUNDS)
        {
            u16 screenWidth = templates[i].screenWidth;
            u16 screenHeight = templates[i].screenHeight;

            if (screenWidth == 0)
                screenWidth = 256;
            if (screenHeight == 0)
                screenHeight = 256;

            SetBgControlAttributes(bg,
                                   templates[i].charBaseIndex,
                                   templates[i].mapBaseIndex,
                                   screenWidth,
                                   screenHeight,
                                   templates[i].paletteMode,
                                   1,
                                   templates[i].priority,
                                   0,
                                   0);

            sGpuBgConfigs2[bg].baseTile = templates[i].baseTile;
            sGpuBgConfigs2[bg].basePalette = 0;

            sGpuBgConfigs2[bg].tilemap = NULL;
            sGpuBgConfigs2[bg].bg_x = 0;
            sGpuBgConfigs2[bg].bg_y = 0;
        }
    }
}

void InitBgFromTemplate(const struct BgTemplate *template)
{
    u8 bg = template->bg;

    if (bg < NUM_BACKGROUNDS)
    {
        u16 screenWidth = template->screenWidth;
        u16 screenHeight = template->screenHeight;

        if (screenWidth == 0)
            screenWidth = 256;
        if (screenHeight == 0)
            screenHeight = 256;

        SetBgControlAttributes(bg,
                               template->charBaseIndex,
                               template->mapBaseIndex,
                               screenWidth,
                               screenHeight,
                               template->paletteMode,
                               1,
                               template->priority,
                               0,
                               0);

        sGpuBgConfigs2[bg].baseTile = template->baseTile;
        sGpuBgConfigs2[bg].basePalette = 0;

        sGpuBgConfigs2[bg].tilemap = NULL;
        sGpuBgConfigs2[bg].bg_x = 0;
        sGpuBgConfigs2[bg].bg_y = 0;
    }
}

void SetBgMode(u8 bgMode)
{
    SetBgModeInternal(bgMode);
}

u16 LoadBgTiles(u8 bg, const void *src, size_t size, size_t destOffset)
{
    size_t tileOffset;
    u8 cursor;

    if (GetBgControlAttribute(bg, BG_CTRL_ATTR_PALETTEMODE) == 0)
    {
        tileOffset = (sGpuBgConfigs2[bg].baseTile + destOffset) * 0x20;
    }
    else
    {
        tileOffset = (sGpuBgConfigs2[bg].baseTile + destOffset) * 0x40;
    }

    cursor = LoadBgVram(bg, src, size, tileOffset, 0x1);

    if (cursor == 0xFF)
    {
        return -1;
    }

    if (gWindowTileAutoAllocEnabled == TRUE)
        BgTileAllocOp(bg, tileOffset / 0x20, size / 0x20, 1);

    return cursor;
}

u16 LoadBgTilemap(u8 bg, const void *src, size_t size, size_t destOffset)
{
    u8 cursor = LoadBgVram(bg, src, size, destOffset * 2, 0x2);

    if (cursor == 0xFF)
    {
        return -1;
    }

    return cursor;
}

u16 Unused_LoadBgPalette(u8 bg, const void *src, u16 size, u16 destOffset)
{
    s8 cursor;

    if (!IsInvalidBg32(bg))
    {
        u16 paletteOffset = PLTT_OFFSET_4BPP(sGpuBgConfigs2[bg].basePalette) + (destOffset * 2);
        cursor = RequestDma3Copy(src, (void *)(paletteOffset + BG_PLTT), size, 0);

        if (cursor == -1)
            return -1;
    }
    else
    {
        return -1;
    }

    return (u8)cursor;
}

bool8 IsDma3ManagerBusyWithBgCopy(void)
{
    // TODO: Remove this.
    return FALSE;
}

void ShowBg(u8 bg)
{
    ShowBgInternal(bg);
    SyncBgVisibilityAndMode();
}

void HideBg(u8 bg)
{
    HideBgInternal(bg);
    SyncBgVisibilityAndMode();
}

void SetBgAttribute(u8 bg, u8 attributeId, u16 value)
{
    if (IsInvalidBg(bg))
        return;

    switch (attributeId)
    {
    case BG_ATTR_CHARBASEINDEX:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].charBaseIndex = value;
        break;
    case BG_ATTR_MAPBASEINDEX:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].mapBaseIndex = value;
        break;
    case BG_ATTR_SCREENWIDTH:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].screenWidth = value;
        break;
    case BG_ATTR_SCREENHEIGHT:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].screenHeight = value;
        break;
    case BG_ATTR_PALETTEMODE:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].paletteMode = value;
        break;
    case BG_ATTR_GBAMODE:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].gbaMode = value;
        break;
    case BG_ATTR_PRIORITY:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].priority = value;
        break;
    case BG_ATTR_MOSAIC:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].mosaic = value;
        break;
    case BG_ATTR_WRAPAROUND:
        if (value != 0xFFFF)
            sGpuBgConfigs.configs[bg].wraparound = value;
        break;
    }

    sGpuBgConfigs.configs[bg].visible = 1;
}

int GetBgAttribute(u8 bg, u8 attributeId)
{
    switch (attributeId)
    {
    case BG_ATTR_CHARBASEINDEX:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_CHARBASEINDEX);
    case BG_ATTR_MAPBASEINDEX:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_MAPBASEINDEX);
    case BG_ATTR_SCREENWIDTH:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENWIDTH);
    case BG_ATTR_SCREENHEIGHT:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENHEIGHT);
    case BG_ATTR_PALETTEMODE:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_PALETTEMODE);
    case BG_ATTR_GBAMODE:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_GBAMODE);
    case BG_ATTR_PRIORITY:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_PRIORITY);
    case BG_ATTR_MOSAIC:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_MOSAIC);
    case BG_ATTR_WRAPAROUND:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_WRAPAROUND);
    case BG_ATTR_METRIC:
        switch (GetBgType(bg))
        {
        case BG_TYPE_NORMAL:
            return GetBgMetricTextMode(bg, 0);
        case BG_TYPE_AFFINE:
            return GetBgMetricAffineMode(bg, 0);
        default:
            return 0;
        }
    case BG_ATTR_TYPE:
        return GetBgType(bg);
    case BG_ATTR_BASETILE:
        return sGpuBgConfigs2[bg].baseTile;
    default:
        return -1;
    }
}

s32 ChangeBgX(u8 bg, s32 value, u8 op)
{
    if (IsInvalidBg32(bg) || !GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
    {
        return -1;
    }

    switch (op)
    {
    case BG_COORD_SET:
    default:
        sGpuBgConfigs2[bg].bg_x = value;
        break;
    case BG_COORD_ADD:
        sGpuBgConfigs2[bg].bg_x += value;
        break;
    case BG_COORD_SUB:
        sGpuBgConfigs2[bg].bg_x -= value;
        break;
    }

    u8 isAffine = GetGpuBackgroundAffine(bg);
    if (isAffine)
    {
        u32 frac = sGpuBgConfigs2[2].bg_x & 0xFF;
        u32 whole = (sGpuBgConfigs2[2].bg_x & 0xFFFFF00) >> 4;
        SetGpuBackgroundX(bg, frac | whole);
    }
    else
    {
        SetGpuBackgroundX(bg, sGpuBgConfigs2[bg].bg_x >> 0x8);
    }

    return sGpuBgConfigs2[bg].bg_x;
}

s32 GetBgX(u8 bg)
{
    if (IsInvalidBg32(bg))
        return -1;
    else if (!GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
        return -1;
    else
        return sGpuBgConfigs2[bg].bg_x;
}

s32 ChangeBgY(u8 bg, s32 value, u8 op)
{
    u8 mode;
    u16 temp1;
    u16 temp2;

    if (IsInvalidBg32(bg) || !GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
    {
        return -1;
    }

    switch (op)
    {
    case BG_COORD_SET:
    default:
        sGpuBgConfigs2[bg].bg_y = value;
        break;
    case BG_COORD_ADD:
        sGpuBgConfigs2[bg].bg_y += value;
        break;
    case BG_COORD_SUB:
        sGpuBgConfigs2[bg].bg_y -= value;
        break;
    }

    u8 isAffine = GetGpuBackgroundAffine(bg);
    if (isAffine)
    {
        u32 frac = sGpuBgConfigs2[2].bg_y & 0xFF;
        u32 whole = (sGpuBgConfigs2[2].bg_y & 0xFFFFF00) >> 4;
        SetGpuBackgroundY(bg, frac | whole);
    }
    else
    {
        SetGpuBackgroundY(bg, sGpuBgConfigs2[bg].bg_y >> 0x8);
    }

    return sGpuBgConfigs2[bg].bg_y;
}

s32 GetBgY(u8 bg)
{
    if (IsInvalidBg32(bg))
        return -1;
    else if (!GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
        return -1;
    else
        return sGpuBgConfigs2[bg].bg_y;
}

void SetBgAffine(u8 bg, s32 srcCenterX, s32 srcCenterY, s16 dispCenterX, s16 dispCenterY, s16 scaleX, s16 scaleY, u16 rotationAngle)
{
    SetBgAffineInternal(bg, srcCenterX, srcCenterY, dispCenterX, dispCenterY, scaleX, scaleY, rotationAngle);
}

u8 Unused_AdjustBgMosaic(u8 val, u8 mode)
{
    u16 mosaic = gpu.mosaic;
    s16 bgH = mosaic & 0xF;
    s16 bgV = (mosaic >> 4) & 0xF;

    mosaic &= 0xFF00; // clear background mosaic sizes

    switch (mode)
    {
    case BG_MOSAIC_SET_HV:
    default:
        bgH = val & 0xF;
        bgV = val >> 0x4;
        break;
    case BG_MOSAIC_SET_H:
        bgH = val & 0xF;
        break;
    case BG_MOSAIC_ADD_H:
        if ((bgH + val) > 0xF)
        {
            bgH = 0xF;
        }
        else
        {
            bgH += val;
        }
        break;
    case BG_MOSAIC_SUB_H:
        if ((bgH - val) < 0)
        {
            bgH = 0x0;
        }
        else
        {
            bgH -= val;
        }
        break;
    case BG_MOSAIC_SET_V:
        bgV = val & 0xF;
        break;
    case BG_MOSAIC_ADD_V:
        if ((bgV + val) > 0xF)
        {
            bgV = 0xF;
        }
        else
        {
            bgV += val;
        }
        break;
    case BG_MOSAIC_SUB_V:
        if ((bgV - val) < 0)
        {
            bgV = 0x0;
        }
        else
        {
            bgV -= val;
        }
        break;
    }

    mosaic |= ((bgV << 0x4) & 0xF0);
    mosaic |= (bgH & 0xF);

    SetGpuState(GPU_STATE_MOSAIC, mosaic);

    return mosaic;
}

void SetBgTilemapBuffer(u8 bg, void *tilemap)
{
    if (!IsInvalidBg32(bg) && GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
    {
        sGpuBgConfigs2[bg].tilemap = tilemap;
    }
}

void UnsetBgTilemapBuffer(u8 bg)
{
    if (!IsInvalidBg32(bg) && GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
    {
        sGpuBgConfigs2[bg].tilemap = NULL;
    }
}

void *GetBgTilemapBuffer(u8 bg)
{
    if (IsInvalidBg32(bg))
        return NULL;
    else if (!GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
        return NULL;
    else
        return sGpuBgConfigs2[bg].tilemap;
}

void CopyToBgTilemapBuffer(u8 bg, const void *src, u16 mode, u16 destOffset)
{
    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        if (mode != 0)
            CpuCopy16(src, (void *)(sGpuBgConfigs2[bg].tilemap + (destOffset * 2)), mode);
        else
            LZ77UnCompWram(src, (void *)(sGpuBgConfigs2[bg].tilemap + (destOffset * 2)));
    }
}

void CopyBgTilemapBufferToVram(u8 bg)
{
    size_t sizeToLoad;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        switch (GetBgType(bg))
        {
        case BG_TYPE_NORMAL:
            sizeToLoad = GetBgMetricTextMode(bg, 0);
            break;
        case BG_TYPE_AFFINE:
            sizeToLoad = GetBgMetricAffineMode(bg, 0);
            break;
        default:
            return;
        }
        LoadBgVram(bg, sGpuBgConfigs2[bg].tilemap, sizeToLoad, 0, 2);
    }
}

void CopyToBgTilemapBufferRect(u8 bg, const void *src, u8 destX, u8 destY, u8 width, u8 height)
{
    u16 destX16;
    u16 destY16;
    u16 mode;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        switch (GetBgType(bg))
        {
        case BG_TYPE_NORMAL:
        {
            const u16 * srcCopy = src;
            for (destY16 = destY; destY16 < (destY + height); destY16++)
            {
                for (destX16 = destX; destX16 < (destX + width); destX16++)
                {
                    ((u16 *)sGpuBgConfigs2[bg].tilemap)[((destY16 * 0x20) + destX16)] = *srcCopy++;
                }
            }
            break;
        }
        case BG_TYPE_AFFINE:
        {
            const u8 * srcCopy = src;
            mode = GetBgMetricAffineMode(bg, 0x1);
            for (destY16 = destY; destY16 < (destY + height); destY16++)
            {
                for (destX16 = destX; destX16 < (destX + width); destX16++)
                {
                    ((u8 *)sGpuBgConfigs2[bg].tilemap)[((destY16 * mode) + destX16)] = *srcCopy++;
                }
            }
            break;
        }
        }
    }
}

void CopyToBgTilemapBufferRect_ChangePalette(u8 bg, const void *src, u8 destX, u8 destY, u8 rectWidth, u8 rectHeight, u8 palette)
{
    CopyRectToBgTilemapBufferRect(bg, src, 0, 0, rectWidth, rectHeight, destX, destY, rectWidth, rectHeight, palette, 0, 0);
}

void CopyRectToBgTilemapBufferRect(u8 bg, const void *src, u8 srcX, u8 srcY, u8 srcWidth, u8 srcHeight, u8 destX, u8 destY, u8 rectWidth, u8 rectHeight, u8 palette1, s16 tileOffset, s16 palette2)
{
    u16 screenWidth, screenHeight;
    u16 var;
    const void *srcPtr;
    u16 i, j;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        screenWidth = GetBgMetricTextMode(bg, 0x1);
        screenHeight = GetBgMetricTextMode(bg, 0x2);
        switch (GetBgType(bg))
        {
        case BG_TYPE_NORMAL:
            srcPtr = src + ((srcY * srcWidth) + srcX) * 2;
            for (i = destY; i < (destY + rectHeight); i++)
            {
                for (j = destX; j < (destX + rectWidth); j++)
                {
                    u16 *tilemap = sGpuBgConfigs2[bg].tilemap;
                    u32 index = GetTileMapIndexFromCoords(bg, j, i, screenWidth, screenHeight);
                    CopyTileMapEntry(srcPtr, &tilemap[index], palette1, tileOffset, palette2);
                    srcPtr += 2;
                }
                srcPtr += (srcWidth - rectWidth) * 2;
            }
            break;
        case BG_TYPE_AFFINE:
            srcPtr = src + ((srcY * srcWidth) + srcX);
            var = GetBgMetricAffineMode(bg, 0x1);
            for (i = destY; i < (destY + rectHeight); i++)
            {
                for (j = destX; j < (destX + rectWidth); j++)
                {
                    u8 *tilemap = sGpuBgConfigs2[bg].tilemap;
                    tilemap[(i * var) + j] = *(u8 *)(srcPtr) + tileOffset;
                    srcPtr++;
                }
                srcPtr += (srcWidth - rectWidth);
            }
            break;
        }
    }
}

void FillBgTilemapBufferRect_Palette0(u8 bg, u16 tileNum, u8 x, u8 y, u8 width, u8 height)
{
    u16 x16;
    u16 y16;
    u16 mode;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        switch (GetBgType(bg))
        {
        case BG_TYPE_NORMAL:
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    ((u16 *)sGpuBgConfigs2[bg].tilemap)[((y16 * 0x20) + x16)] = tileNum;
                }
            }
            break;
        case BG_TYPE_AFFINE:
            mode = GetBgMetricAffineMode(bg, 0x1);
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    ((u8 *)sGpuBgConfigs2[bg].tilemap)[((y16 * mode) + x16)] = tileNum;
                }
            }
            break;
        }
    }
}

void FillBgTilemapBufferRect(u8 bg, u16 tileNum, u8 x, u8 y, u8 width, u8 height, u8 palette)
{
    WriteSequenceToBgTilemapBuffer(bg, tileNum, x, y, width, height, palette, 0);
}

void WriteSequenceToBgTilemapBuffer(u8 bg, u16 firstTileNum, u8 x, u8 y, u8 width, u8 height, u8 paletteSlot, s16 tileNumDelta)
{
    u16 screenWidth;
    u16 screenHeight;
    u16 x16, y16;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        screenWidth = GetBgMetricTextMode(bg, 0x1);
        screenHeight = GetBgMetricTextMode(bg, 0x2);
        switch (GetBgType(bg))
        {
        case BG_TYPE_NORMAL:
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    u16 *tilemap = sGpuBgConfigs2[bg].tilemap;
                    u32 index = GetTileMapIndexFromCoords(bg, x16, y16, screenWidth, screenHeight);
                    CopyTileMapEntry(&firstTileNum, &tilemap[index], paletteSlot, 0, 0);
                    firstTileNum = (firstTileNum & 0xFC00) + ((firstTileNum + tileNumDelta) & 0x3FF);
                }
            }
            break;
        case BG_TYPE_AFFINE:
            screenWidth = GetBgMetricAffineMode(bg, 0x1);
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    u8 *tilemap = sGpuBgConfigs2[bg].tilemap;
                    tilemap[(y16 * screenWidth) + x16] = firstTileNum;
                    firstTileNum = (firstTileNum & 0xFC00) + ((firstTileNum + tileNumDelta) & 0x3FF);
                }
            }
            break;
        }
    }
}

u32 GetBgMetricTextMode(u8 bg, u8 whichMetric)
{
    int screenSize = -1;
    int screenWidth = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENWIDTH);
    int screenHeight = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENHEIGHT);

    // Emulate GBA screen sizes
    if (GetBgControlAttribute(bg, BG_CTRL_ATTR_GBAMODE))
    {
        if (screenWidth == 256 && screenHeight == 256)
        {
            screenSize = 0;
        }
        else if (screenWidth == 512 && screenHeight == 256)
        {
            screenSize = 1;
        }
        else if (screenWidth == 256 && screenHeight == 512)
        {
            screenSize = 2;
        }
        else if (screenWidth == 512 && screenHeight == 512)
        {
            screenSize = 3;
        }
    }

    switch (whichMetric)
    {
    case 0:
        switch (screenSize)
        {
        case 0:
            return 1 * GBA_BG_SCREEN_SIZE;
        case 1:
        case 2:
            return 2 * GBA_BG_SCREEN_SIZE;
        case 3:
            return 4 * GBA_BG_SCREEN_SIZE;
        default:
            return (screenWidth * screenHeight) / 8;
            // return BG_SCREEN_SIZE;
        }
        break;
    case 1:
        switch (screenSize)
        {
        case 0:
            return 1 * 0x20;
        case 1:
            return 2 * 0x20;
        case 2:
            return 1 * 0x20;
        case 3:
            return 2 * 0x20;
        default:
            return screenWidth / 8;
        }
        break;
    case 2:
        switch (screenSize)
        {
        case 0:
        case 1:
            return 1 * 0x20;
        case 2:
        case 3:
            return 2 * 0x20;
        default:
            return screenHeight / 8;
        }
        break;
    }

    return 0;
}

u32 GetBgMetricAffineMode(u8 bg, u8 whichMetric)
{
    int screenSize = -1;

    // Emulate GBA screen modes
    if (GetBgControlAttribute(bg, BG_CTRL_ATTR_GBAMODE))
    {
        int screenWidth = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENWIDTH);
        int screenHeight = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENHEIGHT);
        if (screenWidth == 256 && screenHeight == 256)
        {
            screenSize = 0;
        }
        else if (screenWidth == 512 && screenHeight == 256)
        {
            screenSize = 1;
        }
        else if (screenWidth == 256 && screenHeight == 512)
        {
            screenSize = 2;
        }
        else if (screenWidth == 512 && screenHeight == 512)
        {
            screenSize = 3;
        }
    }

    if (screenSize == -1)
    {
        return GetBgMetricTextMode(bg, whichMetric);
    }

    switch (whichMetric)
    {
    case 0:
        switch (screenSize)
        {
        case 0:
            return 0x1 * 0x100;
        case 1:
            return 0x4 * 0x100;
        case 2:
            return 0x10 * 0x100;
        case 3:
            return 0x40 * 0x100;
        }
        break;
    case 1:
    case 2:
        return 0x10 << screenSize;
    }

    return 0;
}

u32 GetTileMapIndexFromCoords(u8 bg, s32 x, s32 y, u32 screenWidth, u32 screenHeight)
{
    // Emulate GBA screen modes
    if (GetBgControlAttribute(bg, BG_CTRL_ATTR_GBAMODE))
    {
        int screenSize = -1;

        if (screenWidth * 8 == 256 && screenHeight * 8 == 256)
        {
            screenSize = 0;
        }
        else if (screenWidth * 8 == 512 && screenHeight * 8 == 256)
        {
            screenSize = 1;
        }
        else if (screenWidth * 8 == 256 && screenHeight * 8 == 512)
        {
            screenSize = 2;
        }
        else if (screenWidth * 8 == 512 && screenHeight * 8 == 512)
        {
            screenSize = 3;
        }

        x = x & (screenWidth - 1);
        y = y & (screenHeight - 1);

        switch (screenSize)
        {
        case 0:
        case 2:
            break;
        case 3:
            if (y >= 0x20)
                y += 0x20;
        case 1:
            if (x >= 0x20)
            {
                x -= 0x20;
                y += 0x20;
            }
            break;
        }

        return (y * 0x20) + x;
    }

    x %= screenWidth;
    y %= screenHeight;

    return (y * screenWidth) + x;
}

void CopyTileMapEntry(const u16 *src, u16 *dest, s32 palette1, s32 tileOffset, s32 palette2)
{
    u16 var;

    switch (palette1)
    {
    case 0 ... 15:
        var = ((*src + tileOffset) & 0xFFF) + ((palette1 + palette2) << 12);
        break;
    case 16:
        var = *dest;
        var &= 0xFC00;
        var += palette2 << 12;
        var |= (*src + tileOffset) & 0x3FF;
        break;
    default:
    case 17 ... INT_MAX:
        var = *src + tileOffset + (palette2 << 12);
        break;
    }
    *dest = var;
}

static u32 GetBgType(u8 bg)
{
    if (bg >= NUM_BACKGROUNDS)
        return BG_TYPE_NONE;

    u8 isAffine = GetGpuBackgroundAffine(bg);
    if (isAffine)
        return BG_TYPE_AFFINE;

    return BG_TYPE_NORMAL;
}

bool32 IsInvalidBg32(u8 bg)
{
    if (bg >= NUM_BACKGROUNDS)
        return TRUE;
    else
        return FALSE;
}

bool32 IsTileMapOutsideWram(u8 bg)
{
    return sGpuBgConfigs2[bg].tilemap == NULL;
}
