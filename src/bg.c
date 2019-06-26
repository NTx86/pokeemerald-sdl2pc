#include "global.h"
#include "bg.h"
#include "dma3.h"
#include "gpu_regs.h"

#define DISPCNT_ALL_BG_AND_MODE_BITS    (DISPCNT_BG_ALL_ON | 0x7)

struct BgControl
{
    struct BgConfig {
        u16 visible:1;
        u16 unknown_1:1;
        u16 screenSize:2;
        u16 priority:2;
        u16 mosaic:1;
        u16 wraparound:1;

        u16 charBaseIndex:2;
        u16 mapBaseIndex:5;
        u16 paletteMode:1;

        u8 unknown_2;
        u8 unknown_3;
    } configs[4];

    u16 bgVisibilityAndMode;
};

struct BgConfig2
{
    u32 baseTile:10;
    u32 basePalette:4;
    u32 unk_3:18;

    void* tilemap;
    s32 bg_x;
    s32 bg_y;
};

static IWRAM_DATA struct BgControl sGpuBgConfigs;
static IWRAM_DATA struct BgConfig2 sGpuBgConfigs2[4];
static IWRAM_DATA u32 sDmaBusyBitfield[4];

u32 gUnneededFireRedVariable;

static const struct BgConfig sZeroedBgControlStruct = { 0 };

void ResetBgs(void)
{
    ResetBgControlStructs();
    sGpuBgConfigs.bgVisibilityAndMode = 0;
    SetTextModeAndHideBgs();
}

static void SetBgModeInternal(u8 bgMode)
{
    sGpuBgConfigs.bgVisibilityAndMode &= 0xFFF8;
    sGpuBgConfigs.bgVisibilityAndMode |= bgMode;
}

u8 GetBgMode(void)
{
    return sGpuBgConfigs.bgVisibilityAndMode & 0x7;
}

void ResetBgControlStructs(void)
{
    struct BgConfig* bgConfigs = &sGpuBgConfigs.configs[0];
    struct BgConfig zeroedConfig = sZeroedBgControlStruct;
    int i;

    for (i = 0; i < 4; i++)
    {
        bgConfigs[i] = zeroedConfig;
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
    BG_CTRL_ATTR_SCREENSIZE = 4,
    BG_CTRL_ATTR_PALETTEMODE = 5,
    BG_CTRL_ATTR_PRIORITY = 6,
    BG_CTRL_ATTR_MOSAIC = 7,
    BG_CTRL_ATTR_WRAPAROUND = 8,
};

static void SetBgControlAttributes(u8 bg, u8 charBaseIndex, u8 mapBaseIndex, u8 screenSize, u8 paletteMode, u8 priority, u8 mosaic, u8 wraparound)
{
    if (!IsInvalidBg(bg))
    {
        if (charBaseIndex != 0xFF)
        {
            sGpuBgConfigs.configs[bg].charBaseIndex = charBaseIndex & 0x3;
        }

        if (mapBaseIndex != 0xFF)
        {
            sGpuBgConfigs.configs[bg].mapBaseIndex = mapBaseIndex & 0x1F;
        }

        if (screenSize != 0xFF)
        {
            sGpuBgConfigs.configs[bg].screenSize = screenSize & 0x3;
        }

        if (paletteMode != 0xFF)
        {
            sGpuBgConfigs.configs[bg].paletteMode = paletteMode;
        }

        if (priority != 0xFF)
        {
            sGpuBgConfigs.configs[bg].priority = priority & 0x3;
        }

        if (mosaic != 0xFF)
        {
            sGpuBgConfigs.configs[bg].mosaic = mosaic & 0x1;
        }

        if (wraparound != 0xFF)
        {
            sGpuBgConfigs.configs[bg].wraparound = wraparound;
        }

        sGpuBgConfigs.configs[bg].unknown_2 = 0;
        sGpuBgConfigs.configs[bg].unknown_3 = 0;

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
        case BG_CTRL_ATTR_SCREENSIZE:
            return sGpuBgConfigs.configs[bg].screenSize;
        case BG_CTRL_ATTR_PALETTEMODE:
            return sGpuBgConfigs.configs[bg].paletteMode;
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

u8 LoadBgVram(u8 bg, const void *src, u16 size, u16 destOffset, u8 mode)
{
    u16 offset;
    s8 cursor;

    if (!IsInvalidBg(bg) && sGpuBgConfigs.configs[bg].visible)
    {
        switch (mode)
        {
        case 0x1:
            offset = sGpuBgConfigs.configs[bg].charBaseIndex * BG_CHAR_SIZE;
            break;
        case 0x2:
            offset = sGpuBgConfigs.configs[bg].mapBaseIndex * BG_SCREEN_SIZE;
            break;
        default:
            cursor = -1;
            goto end;
        }

        offset = destOffset + offset;

        cursor = RequestDma3Copy(src, (void*)(offset + BG_VRAM), size, 0);

        if (cursor == -1)
        {
            return -1;
        }
    }
    else
    {
       return -1;
    }

end:
    return cursor;
}

static void ShowBgInternal(u8 bg)
{
    u16 value;
    if (!IsInvalidBg(bg) && sGpuBgConfigs.configs[bg].visible)
    {
        value = sGpuBgConfigs.configs[bg].priority |
                (sGpuBgConfigs.configs[bg].charBaseIndex << 2) |
                (sGpuBgConfigs.configs[bg].mosaic << 6) |
                (sGpuBgConfigs.configs[bg].paletteMode << 7) |
                (sGpuBgConfigs.configs[bg].mapBaseIndex << 8) |
                (sGpuBgConfigs.configs[bg].wraparound << 13) |
                (sGpuBgConfigs.configs[bg].screenSize << 14);

        SetGpuReg((bg << 1) + 0x8, value);

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
    SetGpuReg(REG_OFFSET_DISPCNT, (GetGpuReg(REG_OFFSET_DISPCNT) & ~DISPCNT_ALL_BG_AND_MODE_BITS) | sGpuBgConfigs.bgVisibilityAndMode);
}

void SetTextModeAndHideBgs(void)
{
    SetGpuReg(REG_OFFSET_DISPCNT, GetGpuReg(REG_OFFSET_DISPCNT) & ~DISPCNT_ALL_BG_AND_MODE_BITS);
}

static void SetBgAffineInternal(u8 bg, s32 srcCenterX, s32 srcCenterY, s16 dispCenterX, s16 dispCenterY, s16 scaleX, s16 scaleY, u16 rotationAngle)
{
    struct BgAffineSrcData src;
    struct BgAffineDstData dest;

    switch (sGpuBgConfigs.bgVisibilityAndMode & 0x7)
    {
    case 1:
        if (bg != 2)
            return;
        break;
    case 2:
        if (bg < 2 || bg > 3)
            return;
        break;
    case 0:
    default:
        return;
    }

    src.texX = srcCenterX;
    src.texY = srcCenterY;
    src.scrX = dispCenterX;
    src.scrY = dispCenterY;
    src.sx = scaleX;
    src.sy = scaleY;
    src.alpha = rotationAngle;

    BgAffineSet(&src, &dest, 1);

    SetGpuReg(REG_OFFSET_BG2PA, dest.pa);
    SetGpuReg(REG_OFFSET_BG2PB, dest.pb);
    SetGpuReg(REG_OFFSET_BG2PC, dest.pc);
    SetGpuReg(REG_OFFSET_BG2PD, dest.pd);
    SetGpuReg(REG_OFFSET_BG2PA, dest.pa);
    SetGpuReg(REG_OFFSET_BG2X_L, (s16)(dest.dx));
    SetGpuReg(REG_OFFSET_BG2X_H, (s16)(dest.dx >> 16));
    SetGpuReg(REG_OFFSET_BG2Y_L, (s16)(dest.dy));
    SetGpuReg(REG_OFFSET_BG2Y_H, (s16)(dest.dy >> 16));
}

bool8 IsInvalidBg(u8 bg)
{
    if (bg > 3)
        return TRUE;
    else
        return FALSE;
}

int DummiedOutFireRedLeafGreenTileAllocFunc(int a1, int a2, int a3, int a4)
{
    return 0;
}

void ResetBgsAndClearDma3BusyFlags(u32 leftoverFireRedLeafGreenVariable)
{
    int i;
    ResetBgs();

    for (i = 0; i < 4; i++)
    {
        sDmaBusyBitfield[i] = 0;
    }

    gUnneededFireRedVariable = leftoverFireRedLeafGreenVariable;
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
        if (bg < 4)
        {
            SetBgControlAttributes(bg,
                                   templates[i].charBaseIndex,
                                   templates[i].mapBaseIndex,
                                   templates[i].screenSize,
                                   templates[i].paletteMode,
                                   templates[i].priority,
                                   0,
                                   0);

            sGpuBgConfigs2[bg].baseTile = templates[i].baseTile;
            sGpuBgConfigs2[bg].basePalette = 0;
            sGpuBgConfigs2[bg].unk_3 = 0;

            sGpuBgConfigs2[bg].tilemap = NULL;
            sGpuBgConfigs2[bg].bg_x = 0;
            sGpuBgConfigs2[bg].bg_y = 0;
        }
    }
}

void InitBgFromTemplate(const struct BgTemplate *template)
{
    u8 bg = template->bg;

    if (bg < 4)
    {
        SetBgControlAttributes(bg,
                               template->charBaseIndex,
                               template->mapBaseIndex,
                               template->screenSize,
                               template->paletteMode,
                               template->priority,
                               0,
                               0);

        sGpuBgConfigs2[bg].baseTile = template->baseTile;
        sGpuBgConfigs2[bg].basePalette = 0;
        sGpuBgConfigs2[bg].unk_3 = 0;

        sGpuBgConfigs2[bg].tilemap = NULL;
        sGpuBgConfigs2[bg].bg_x = 0;
        sGpuBgConfigs2[bg].bg_y = 0;
    }
}

void SetBgMode(u8 bgMode)
{
    SetBgModeInternal(bgMode);
}

u16 LoadBgTiles(u8 bg, const void* src, u16 size, u16 destOffset)
{
    u16 tileOffset;
    u8 cursor;

    if (GetBgControlAttribute(bg, BG_CTRL_ATTR_PALETTEMODE) == 0)
    {
        tileOffset = (sGpuBgConfigs2[bg].baseTile + destOffset) * 0x20;
    }
    else
    {
        tileOffset = (sGpuBgConfigs2[bg].baseTile + destOffset) * 0x40;
    }

    cursor = LoadBgVram(bg, src, size, tileOffset, DISPCNT_MODE_1);

    if (cursor == 0xFF)
    {
        return -1;
    }

    sDmaBusyBitfield[cursor / 0x20] |= (1 << (cursor % 0x20));

    if (gUnneededFireRedVariable == 1)
    {
        DummiedOutFireRedLeafGreenTileAllocFunc(bg, tileOffset / 0x20, size / 0x20, 1);
    }

    return cursor;
}

u16 LoadBgTilemap(u8 bg, const void *src, u16 size, u16 destOffset)
{
    u8 cursor = LoadBgVram(bg, src, size, destOffset * 2, DISPCNT_MODE_2);

    if (cursor == 0xFF)
    {
        return -1;
    }

    sDmaBusyBitfield[cursor / 0x20] |= (1 << (cursor % 0x20));

    return cursor;
}

u16 Unused_LoadBgPalette(u8 bg, const void *src, u16 size, u16 destOffset)
{
    s8 cursor;

    if (!IsInvalidBg32(bg))
    {
        u16 paletteOffset = (sGpuBgConfigs2[bg].basePalette * 0x20) + (destOffset * 2);
        cursor = RequestDma3Copy(src, (void*)(paletteOffset + BG_PLTT), size, 0);

        if (cursor == -1)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    sDmaBusyBitfield[cursor / 0x20] |= (1 << (cursor % 0x20));

    return (u8)cursor;
}

bool8 IsDma3ManagerBusyWithBgCopy(void)
{
    int i;

    for (i = 0; i < 0x80; i++)
    {
        u8 div = i / 0x20;
        u8 mod = i % 0x20;

        if ((sDmaBusyBitfield[div] & (1 << mod)))
        {
            s8 reqSpace = CheckForSpaceForDma3Request(i);
            if (reqSpace == -1)
            {
                return TRUE;
            }

            sDmaBusyBitfield[div] &= ~(1 << mod);
        }
    }

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

void SetBgAttribute(u8 bg, u8 attributeId, u8 value)
{
    switch (attributeId)
    {
    case BG_ATTR_CHARBASEINDEX:
        SetBgControlAttributes(bg, value, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
        break;
    case BG_ATTR_MAPBASEINDEX:
        SetBgControlAttributes(bg, 0xFF, value, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
        break;
    case BG_ATTR_SCREENSIZE:
        SetBgControlAttributes(bg, 0xFF, 0xFF, value, 0xFF, 0xFF, 0xFF, 0xFF);
        break;
    case BG_ATTR_PALETTEMODE:
        SetBgControlAttributes(bg, 0xFF, 0xFF, 0xFF, value, 0xFF, 0xFF, 0xFF);
        break;
    case BG_ATTR_PRIORITY:
        SetBgControlAttributes(bg, 0xFF, 0xFF, 0xFF, 0xFF, value, 0xFF, 0xFF);
        break;
    case BG_ATTR_MOSAIC:
        SetBgControlAttributes(bg, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, value, 0xFF);
        break;
    case BG_ATTR_WRAPAROUND:
        SetBgControlAttributes(bg, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, value);
        break;
    }
}

u16 GetBgAttribute(u8 bg, u8 attributeId)
{
    switch (attributeId)
    {
    case BG_ATTR_CHARBASEINDEX:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_CHARBASEINDEX);
    case BG_ATTR_MAPBASEINDEX:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_MAPBASEINDEX);
    case BG_ATTR_SCREENSIZE:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENSIZE);
    case BG_ATTR_PALETTEMODE:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_PALETTEMODE);
    case BG_ATTR_PRIORITY:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_PRIORITY);
    case BG_ATTR_MOSAIC:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_MOSAIC);
    case BG_ATTR_WRAPAROUND:
        return GetBgControlAttribute(bg, BG_CTRL_ATTR_WRAPAROUND);
    case BG_ATTR_METRIC:
        switch (GetBgType(bg))
        {
        case 0:
            return GetBgMetricTextMode(bg, 0) * 0x800;
        case 1:
            return GetBgMetricAffineMode(bg, 0) * 0x100;
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
    u8 mode;
    u16 temp1;
    u16 temp2;

    if (IsInvalidBg32(bg) || !GetBgControlAttribute(bg, BG_CTRL_ATTR_VISIBLE))
    {
        return -1;
    }

    switch (op)
    {
    case 0:
    default:
        sGpuBgConfigs2[bg].bg_x = value;
        break;
    case 1:
        sGpuBgConfigs2[bg].bg_x += value;
        break;
    case 2:
        sGpuBgConfigs2[bg].bg_x -= value;
        break;
    }

    mode = GetBgMode();

    switch (bg)
    {
    case 0:
        temp1 = sGpuBgConfigs2[0].bg_x >> 0x8;
        SetGpuReg(REG_OFFSET_BG0HOFS, temp1);
        break;
    case 1:
        temp1 = sGpuBgConfigs2[1].bg_x >> 0x8;
        SetGpuReg(REG_OFFSET_BG1HOFS, temp1);
        break;
    case 2:
        if (mode == 0)
        {
            temp1 = sGpuBgConfigs2[2].bg_x >> 0x8;
            SetGpuReg(REG_OFFSET_BG2HOFS, temp1);
        }
        else
        {
            temp1 = sGpuBgConfigs2[2].bg_x >> 0x10;
            temp2 = sGpuBgConfigs2[2].bg_x & 0xFFFF;
            SetGpuReg(REG_OFFSET_BG2X_H, temp1);
            SetGpuReg(REG_OFFSET_BG2X_L, temp2);
        }
        break;
    case 3:
        if (mode == 0)
        {
            temp1 = sGpuBgConfigs2[3].bg_x >> 0x8;
            SetGpuReg(REG_OFFSET_BG3HOFS, temp1);
        }
        else if (mode == 2)
        {
            temp1 = sGpuBgConfigs2[3].bg_x >> 0x10;
            temp2 = sGpuBgConfigs2[3].bg_x & 0xFFFF;
            SetGpuReg(REG_OFFSET_BG3X_H, temp1);
            SetGpuReg(REG_OFFSET_BG3X_L, temp2);
        }
        break;
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
    case 0:
    default:
        sGpuBgConfigs2[bg].bg_y = value;
        break;
    case 1:
        sGpuBgConfigs2[bg].bg_y += value;
        break;
    case 2:
        sGpuBgConfigs2[bg].bg_y -= value;
        break;
    }

    mode = GetBgMode();

    switch (bg)
    {
    case 0:
        temp1 = sGpuBgConfigs2[0].bg_y >> 0x8;
        SetGpuReg(REG_OFFSET_BG0VOFS, temp1);
        break;
    case 1:
        temp1 = sGpuBgConfigs2[1].bg_y >> 0x8;
        SetGpuReg(REG_OFFSET_BG1VOFS, temp1);
        break;
    case 2:
        if (mode == 0)
        {
            temp1 = sGpuBgConfigs2[2].bg_y >> 0x8;
            SetGpuReg(REG_OFFSET_BG2VOFS, temp1);
        }
        else
        {
            temp1 = sGpuBgConfigs2[2].bg_y >> 0x10;
            temp2 = sGpuBgConfigs2[2].bg_y & 0xFFFF;
            SetGpuReg(REG_OFFSET_BG2Y_H, temp1);
            SetGpuReg(REG_OFFSET_BG2Y_L, temp2);
        }
        break;
    case 3:
        if (mode == 0)
        {
            temp1 = sGpuBgConfigs2[3].bg_y >> 0x8;
            SetGpuReg(REG_OFFSET_BG3VOFS, temp1);
        }
        else if (mode == 2)
        {
            temp1 = sGpuBgConfigs2[3].bg_y >> 0x10;
            temp2 = sGpuBgConfigs2[3].bg_y & 0xFFFF;
            SetGpuReg(REG_OFFSET_BG3Y_H, temp1);
            SetGpuReg(REG_OFFSET_BG3Y_L, temp2);
        }
        break;
    }

    return sGpuBgConfigs2[bg].bg_y;
}

s32 ChangeBgY_ScreenOff(u8 bg, u32 value, u8 op)
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
    case 0:
    default:
        sGpuBgConfigs2[bg].bg_y = value;
        break;
    case 1:
        sGpuBgConfigs2[bg].bg_y += value;
        break;
    case 2:
        sGpuBgConfigs2[bg].bg_y -= value;
        break;
    }

    mode = GetBgMode();

    switch (bg)
    {
    case 0:
        temp1 = sGpuBgConfigs2[0].bg_y >> 0x8;
        SetGpuReg_ForcedBlank(REG_OFFSET_BG0VOFS, temp1);
        break;
    case 1:
        temp1 = sGpuBgConfigs2[1].bg_y >> 0x8;
        SetGpuReg_ForcedBlank(REG_OFFSET_BG1VOFS, temp1);
        break;
    case 2:
        if (mode == 0)
        {
            temp1 = sGpuBgConfigs2[2].bg_y >> 0x8;
            SetGpuReg_ForcedBlank(REG_OFFSET_BG2VOFS, temp1);

        }
        else
        {
            temp1 = sGpuBgConfigs2[2].bg_y >> 0x10;
            temp2 = sGpuBgConfigs2[2].bg_y & 0xFFFF;
            SetGpuReg_ForcedBlank(REG_OFFSET_BG2Y_H, temp1);
            SetGpuReg_ForcedBlank(REG_OFFSET_BG2Y_L, temp2);
        }
        break;
    case 3:
        if (mode == 0)
        {
            temp1 = sGpuBgConfigs2[3].bg_y >> 0x8;
            SetGpuReg_ForcedBlank(REG_OFFSET_BG3VOFS, temp1);
        }
        else if (mode == 2)
        {
            temp1 = sGpuBgConfigs2[3].bg_y >> 0x10;
            temp2 = sGpuBgConfigs2[3].bg_y & 0xFFFF;
            SetGpuReg_ForcedBlank(REG_OFFSET_BG3Y_H, temp1);
            SetGpuReg_ForcedBlank(REG_OFFSET_BG3Y_L, temp2);
        }
        break;
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

u8 Unused_AdjustBgMosaic(u8 a1, u8 a2)
{
    u16 result = GetGpuReg(REG_OFFSET_MOSAIC);
    s16 test1 = result & 0xF;
    s16 test2 = (result >> 4) & 0xF;

    result &= 0xFF00;

    switch (a2)
    {
    case 0:
    default:
        test1 = a1 & 0xF;
        test2 = a1 >> 0x4;
        break;
    case 1:
        test1 = a1 & 0xF;
        break;
    case 2:
        if ((test1 + a1) > 0xF)
        {
            test1 = 0xF;
        }
        else
        {
            test1 += a1;
        }
        break;
    case 3:
        if ((test1 - a1) < 0)
        {
            test1 = 0x0;
        }
        else
        {
            test1 -= a1;
        }
        break;
    case 4:
        test2 = a1 & 0xF;
        break;
    case 5:
        if ((test2 + a1) > 0xF)
        {
            test2 = 0xF;
        }
        else
        {
            test2 += a1;
        }
        break;
    case 6:
        if ((test2 - a1) < 0)
        {
            test2 = 0x0;
        }
        else
        {
            test2 -= a1;
        }
        break;
    }

    result |= ((test2 << 0x4) & 0xF0);
    result |= (test1 & 0xF);

    SetGpuReg(REG_OFFSET_MOSAIC, result);

    return result;
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

void* GetBgTilemapBuffer(u8 bg)
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
    u16 sizeToLoad;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        switch (GetBgType(bg))
        {
        case 0:
            sizeToLoad = GetBgMetricTextMode(bg, 0) * 0x800;
            break;
        case 1:
            sizeToLoad = GetBgMetricAffineMode(bg, 0) * 0x100;
            break;
        default:
            sizeToLoad = 0;
            break;
        }
        LoadBgVram(bg, sGpuBgConfigs2[bg].tilemap, sizeToLoad, 0, 2);
    }
}

void CopyToBgTilemapBufferRect(u8 bg, const void* src, u8 destX, u8 destY, u8 width, u8 height)
{
    u16 destX16;
    u16 destY16;
    u16 mode;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        switch (GetBgType(bg))
        {
        case 0:
        {
            const u16 * srcCopy = src;
            for (destY16 = destY; destY16 < (destY + height); destY16++)
            {
                for (destX16 = destX; destX16 < (destX + width); destX16++)
                {
                    ((u16*)sGpuBgConfigs2[bg].tilemap)[((destY16 * 0x20) + destX16)] = *srcCopy++;
                }
            }
            break;
        }
        case 1:
        {
            const u8 * srcCopy = src;
            mode = GetBgMetricAffineMode(bg, 0x1);
            for (destY16 = destY; destY16 < (destY + height); destY16++)
            {
                for (destX16 = destX; destX16 < (destX + width); destX16++)
                {
                    ((u8*)sGpuBgConfigs2[bg].tilemap)[((destY16 * mode) + destX16)] = *srcCopy++;
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

void CopyRectToBgTilemapBufferRect(u8 bg, const void *src, u8 srcX, u8 srcY, u8 srcWidth, u8 unused, u8 srcHeight, u8 destX, u8 destY, u8 rectWidth, u8 rectHeight, s16 palette1, s16 tileOffset)
{
    u16 screenWidth, screenHeight, screenSize;
    u16 var;
    const void *srcPtr;
    u16 i, j;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        screenSize = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENSIZE);
        screenWidth = GetBgMetricTextMode(bg, 0x1) * 0x20;
        screenHeight = GetBgMetricTextMode(bg, 0x2) * 0x20;
        switch (GetBgType(bg))
        {
        case 0:
            srcPtr = src + ((srcY * srcWidth) + srcX) * 2;
            for (i = destX; i < (destX + rectWidth); i++)
            {
                for (j = srcHeight; j < (srcHeight + destY); j++)
                {
                    u16 index = GetTileMapIndexFromCoords(j, i, screenSize, screenWidth, screenHeight);
                    CopyTileMapEntry(srcPtr, sGpuBgConfigs2[bg].tilemap + (index * 2), rectHeight, palette1, tileOffset);
                    srcPtr += 2;
                }
                srcPtr += (srcWidth - destY) * 2;
            }
            break;
        case 1:
            srcPtr = src + ((srcY * srcWidth) + srcX);
            var = GetBgMetricAffineMode(bg, 0x1);
            for (i = destX; i < (destX + rectWidth); i++)
            {
                for (j = srcHeight; j < (srcHeight + destY); j++)
                {
                    *(u8*)(sGpuBgConfigs2[bg].tilemap + ((var * i) + j)) = *(u8*)(srcPtr) + palette1;
                    srcPtr++;
                }
                srcPtr += (srcWidth - destY);
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
        case 0:
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    ((u16*)sGpuBgConfigs2[bg].tilemap)[((y16 * 0x20) + x16)] = tileNum;
                }
            }
            break;
        case 1:
            mode = GetBgMetricAffineMode(bg, 0x1);
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    ((u8*)sGpuBgConfigs2[bg].tilemap)[((y16 * mode) + x16)] = tileNum;
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
    u16 mode;
    u16 mode2;
    u16 attribute;
    u16 mode3;
    u16 x16, y16;

    if (!IsInvalidBg32(bg) && !IsTileMapOutsideWram(bg))
    {
        attribute = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENSIZE);
        mode = GetBgMetricTextMode(bg, 0x1) * 0x20;
        mode2 = GetBgMetricTextMode(bg, 0x2) * 0x20;
        switch (GetBgType(bg))
        {
        case 0:
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    CopyTileMapEntry(&firstTileNum, &((u16*)sGpuBgConfigs2[bg].tilemap)[(u16)GetTileMapIndexFromCoords(x16, y16, attribute, mode, mode2)], paletteSlot, 0, 0);
                    firstTileNum = (firstTileNum & (METATILE_COLLISION_MASK | METATILE_ELEVATION_MASK)) + ((firstTileNum + tileNumDelta) & METATILE_ID_MASK);
                }
            }
            break;
        case 1:
            mode3 = GetBgMetricAffineMode(bg, 0x1);
            for (y16 = y; y16 < (y + height); y16++)
            {
                for (x16 = x; x16 < (x + width); x16++)
                {
                    ((u8*)sGpuBgConfigs2[bg].tilemap)[(y16 * mode3) + x16] = firstTileNum;
                    firstTileNum = (firstTileNum & (METATILE_COLLISION_MASK | METATILE_ELEVATION_MASK)) + ((firstTileNum + tileNumDelta) & METATILE_ID_MASK);
                }
            }
            break;
        }
    }
}

u16 GetBgMetricTextMode(u8 bg, u8 whichMetric)
{
    u8 screenSize = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENSIZE);

    switch (whichMetric)
    {
    case 0:
        switch (screenSize)
        {
        case 0:
            return 1;
        case 1:
        case 2:
            return 2;
        case 3:
            return 4;
        }
        break;
    case 1:
        switch (screenSize)
        {
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 1;
        case 3:
            return 2;
        }
        break;
    case 2:
        switch (screenSize)
        {
        case 0:
        case 1:
            return 1;
        case 2:
        case 3:
            return 2;
        }
        break;
    }
    return 0;
}

u32 GetBgMetricAffineMode(u8 bg, u8 whichMetric)
{
    u8 screenSize = GetBgControlAttribute(bg, BG_CTRL_ATTR_SCREENSIZE);

    switch (whichMetric)
    {
    case 0:
        switch (screenSize)
        {
        case 0:
            return 0x1;
        case 1:
            return 0x4;
        case 2:
            return 0x10;
        case 3:
            return 0x40;
        }
        break;
    case 1:
    case 2:
        return 0x10 << screenSize;
    }
    return 0;
}

u32 GetTileMapIndexFromCoords(s32 x, s32 y, s32 screenSize, u32 screenWidth, u32 screenHeight)
{
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

#ifdef NONMATCHING // This one has some weird switch statement cases that refuse to cooperate
void CopyTileMapEntry(const u16 *src, u16 *dest, s32 palette1, s32 tileOffset, s32 palette2)
{
    u16 var;
    switch (palette1)
    {
    case 0 ... 16:
        if (palette1 != 16)
            var = ((*src + tileOffset) & 0xFFF) + ((palette1 + palette2) << 12);
        else
            var = ((*dest & 0xFC00) + (palette2 << 12)) | ((*src + tileOffset) & 0x3FF);
        break;
    default:
        var = *src + tileOffset + (palette2 << 12);
        break;
    }

    *dest = var;
}
#else
NAKED
void CopyTileMapEntry(const u16 *src, u16 *dest, s32 palette1, s32 tileOffset, s32 palette2)
{
    asm("push {r4-r6,lr}\n\
    add r4, r0, #0\n\
    add r6, r1, #0\n\
    ldr r5, [sp, #0x10]\n\
    cmp r2, #0x10\n\
    beq _08002B14\n\
    cmp r2, #0x10\n\
    bgt _08002B34\n\
    cmp r2, #0\n\
    blt _08002B34\n\
    ldrh r0, [r4]\n\
    add r0, r3\n\
    ldr r3, =0x00000fff\n\
    add r1, r3, #0\n\
    and r0, r1\n\
    add r1, r2, r5\n\
    lsl r1, #12\n\
    b _08002B3A\n\
    .pool\n\
_08002B14:\n\
    ldrh r1, [r6]\n\
    mov r0, #0xFC\n\
    lsl r0, #8\n\
    and r1, r0\n\
    lsl r2, r5, #12\n\
    add r2, r1, r2\n\
    ldrh r0, [r4]\n\
    add r0, r3\n\
    ldr r3, =0x000003ff\n\
    add r1, r3, #0\n\
    and r0, r1\n\
    orr r0, r2\n\
    b _08002B3C\n\
    .pool\n\
_08002B34:\n\
    ldrh r0, [r4]\n\
    add r0, r3\n\
    lsl r1, r5, #12\n\
_08002B3A:\n\
    add r0, r1\n\
_08002B3C:\n\
    lsl r0, #16\n\
    lsr r1, r0, #16\n\
    strh r1, [r6]\n\
    pop {r4-r6}\n\
    pop {r0}\n\
    bx r0\n");
}
#endif // NONMATCHING

u32 GetBgType(u8 bg)
{
    u8 mode = GetBgMode();

    switch (bg)
    {
    case 0:
    case 1:
        switch (mode)
        {
        case 0:
        case 1:
            return 0;
        }
        break;
    case 2:
        switch (mode)
        {
        case 0:
            return 0;
        case 1:
        case 2:
            return 1;
        }
        break;
    case 3:
        switch (mode)
        {
        case 0:
            return 0;
        case 2:
            return 1;
        }
        break;
    }

    return 0xFFFF;
}

bool32 IsInvalidBg32(u8 bg)
{
    if (bg > 3)
        return TRUE;
    else
        return FALSE;
}

bool32 IsTileMapOutsideWram(u8 bg)
{
    if (sGpuBgConfigs2[bg].tilemap > (void*)IWRAM_END)
        return TRUE;
    else if (sGpuBgConfigs2[bg].tilemap == NULL)
        return TRUE;
    else
        return FALSE;
}
