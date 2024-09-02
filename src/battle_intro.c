#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "bg.h"
#include "gpu_regs.h"
#include "main.h"
#include "scanline_effect.h"
#include "task.h"
#include "trig.h"
#include "constants/trainers.h"
#include "gpu_main.h"

static void BattleIntroSlide1(u8);
static void BattleIntroSlide2(u8);
static void BattleIntroSlide3(u8);
static void BattleIntroSlideLink(u8);
static void BattleIntroSlidePartner(u8);

static const TaskFunc sBattleIntroSlideFuncs[] =
{
    [BATTLE_TERRAIN_GRASS]      = BattleIntroSlide1,
    [BATTLE_TERRAIN_LONG_GRASS] = BattleIntroSlide1,
    [BATTLE_TERRAIN_SAND]       = BattleIntroSlide2,
    [BATTLE_TERRAIN_UNDERWATER] = BattleIntroSlide2,
    [BATTLE_TERRAIN_WATER]      = BattleIntroSlide2,
    [BATTLE_TERRAIN_POND]       = BattleIntroSlide1,
    [BATTLE_TERRAIN_MOUNTAIN]   = BattleIntroSlide1,
    [BATTLE_TERRAIN_CAVE]       = BattleIntroSlide1,
    [BATTLE_TERRAIN_BUILDING]   = BattleIntroSlide3,
    [BATTLE_TERRAIN_PLAIN]      = BattleIntroSlide3,
};

void SetAnimBgAttribute(u8 bgId, u8 attributeId, u16 value)
{
    if (bgId < 4)
    {
        switch (attributeId)
        {
        case BG_ANIM_SCREEN_WIDTH:
            SetGpuBackgroundWidth(bgId, value);
            break;
        case BG_ANIM_SCREEN_HEIGHT:
            SetGpuBackgroundHeight(bgId, value);
            break;
        case BG_ANIM_AREA_OVERFLOW_MODE:
            SetGpuBackgroundAreaOverflowMode(bgId, value);
            break;
        case BG_ANIM_MOSAIC:
            SetGpuBackgroundMosaicEnabled(bgId, value);
            break;
        case BG_ANIM_CHAR_BASE_BLOCK:
            SetGpuBackgroundCharBaseBlock(bgId, value);
            break;
        case BG_ANIM_PRIORITY:
            SetGpuBackgroundPriority(bgId, value);
            break;
        case BG_ANIM_PALETTES_MODE:
            SetGpuBackground8bppMode(bgId, value);
            break;
        case BG_ANIM_SCREEN_BASE_BLOCK:
            SetGpuBackgroundScreenBaseBlock(bgId, value);
            break;
        }
    }
}

int GetAnimBgAttribute(u8 bgId, u8 attributeId)
{
    if (bgId < 4)
    {
        switch (attributeId)
        {
        case BG_ANIM_SCREEN_WIDTH:
            return GetGpuBackgroundWidth(bgId);
        case BG_ANIM_SCREEN_HEIGHT:
            return GetGpuBackgroundHeight(bgId);
        case BG_ANIM_AREA_OVERFLOW_MODE:
            return GetGpuBackgroundAreaOverflowMode(bgId);
        case BG_ANIM_MOSAIC:
            return GetGpuBackgroundMosaicEnabled(bgId);
        case BG_ANIM_CHAR_BASE_BLOCK:
            return GetGpuBackgroundCharBaseBlock(bgId);
        case BG_ANIM_PRIORITY:
            return GetGpuBackgroundPriority(bgId);
        case BG_ANIM_PALETTES_MODE:
            return GetGpuBackground8bppMode(bgId);
        case BG_ANIM_SCREEN_BASE_BLOCK:
            return GetGpuBackgroundScreenBaseBlock(bgId);
        }
    }

    return 0;
}

#define tState data[0]
#define tTerrain data[1]

void HandleIntroSlide(u8 terrain)
{
    u8 taskId;

    if ((gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) && gPartnerTrainerId != TRAINER_STEVEN_PARTNER)
    {
        taskId = CreateTask(BattleIntroSlidePartner, 0);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        taskId = CreateTask(BattleIntroSlideLink, 0);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
    {
        taskId = CreateTask(BattleIntroSlide3, 0);
    }
    else if ((gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON) && gGameVersion != VERSION_RUBY)
    {
        terrain = BATTLE_TERRAIN_UNDERWATER;
        taskId = CreateTask(BattleIntroSlide2, 0);
    }
    else
    {
        taskId = CreateTask(sBattleIntroSlideFuncs[terrain], 0);
    }

    gTasks[taskId].tState = 0;
    gTasks[taskId].tTerrain = terrain;
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = 0;
    gTasks[taskId].data[4] = 0;
    gTasks[taskId].data[5] = 0;
    gTasks[taskId].data[6] = 0;
    gTasks[taskId].data[7] = 0;

    EnableBorder();
    SetBorder(GAME_BORDER_BATTLE);
    DoBorderFadeIn(0x100, 0);
}

static void BattleIntroSlideEnd(u8 taskId)
{
    DestroyTask(taskId);
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_WIN0V = WIN_RANGE(0, DisplayHeight() + 1);
    SetGpuState(GPU_STATE_BLDCNT, 0);
    SetGpuState(GPU_STATE_BLDALPHA, 0);
    SetGpuState(GPU_STATE_BLDY, 0);
    SetGpuWindowIn(WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR);
    SetGpuWindowOut(WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR);
}

static void BattleIntroSlide1(u8 taskId)
{
    int i;

    gBattle_BG1_X += 6;
    switch (gTasks[taskId].tState)
    {
    case 0:
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            gTasks[taskId].data[2] = 16;
            gTasks[taskId].tState++;
        }
        else
        {
            gTasks[taskId].data[2] = 1;
            gTasks[taskId].tState++;
        }
        break;
    case 1:
        if (--gTasks[taskId].data[2] == 0)
        {
            gTasks[taskId].tState++;
            SetGpuWindowIn(WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR);
        }
        break;
    case 2:
        gTasks[taskId].data[7] += 1;
        gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        if (gTasks[taskId].data[7] >= 0x30)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].data[2] = DisplayWidth();
            gTasks[taskId].data[3] = 32;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].data[3])
        {
            gTasks[taskId].data[3]--;
        }
        else
        {
            if (gTasks[taskId].tTerrain == BATTLE_TERRAIN_LONG_GRASS)
            {
                if (gBattle_BG1_Y != 0xFFB0)
                    gBattle_BG1_Y -= 2;
            }
            else
            {
                if (gBattle_BG1_Y != 0xFFC8)
                    gBattle_BG1_Y -= 1;
            }
        }

        if (gTasks[taskId].data[7] < DisplayHeight() / 2)
        {
            gTasks[taskId].data[7] += 4;
            gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        }
        else
        {
            gBattle_WIN0V = WIN_RANGE(0, DisplayHeight() + 1);
        }

        if (gTasks[taskId].data[2])
            gTasks[taskId].data[2] -= 2;

        // Scanline settings have already been set in CB2_InitBattleInternal()
        for (i = 0; i < DisplayHeight() / 2; i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = gTasks[taskId].data[2];

        for (; i < DisplayHeight(); i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = -gTasks[taskId].data[2];

        if (gTasks[taskId].data[2] == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
            CpuFill32(0, (void *)BG_SCREEN_ADDR(28), BG_SCREEN_SIZE);
            SetBgAttribute(1, BG_ATTR_CHARBASEINDEX, 0);
            SetBgAttribute(2, BG_ATTR_CHARBASEINDEX, 0);

            ClearGpuBackgroundState(1);
            SetGpuBackgroundScreenBaseBlock(1, 28);
            SetGpuBackgroundWidth(1, 256);
            SetGpuBackgroundHeight(1, 512);

            ClearGpuBackgroundState(2);
            SetGpuBackgroundScreenBaseBlock(2, 30);
            SetGpuBackgroundWidth(2, 512);
            SetGpuBackgroundHeight(2, 256);
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }
}

static void BattleIntroSlide2(u8 taskId)
{
    int i;

    switch (gTasks[taskId].tTerrain)
    {
    case BATTLE_TERRAIN_SAND:
    case BATTLE_TERRAIN_WATER:
        gBattle_BG1_X += 8;
        break;
    case BATTLE_TERRAIN_UNDERWATER:
        gBattle_BG1_X += 6;
        break;
    }

    if (gTasks[taskId].tTerrain == BATTLE_TERRAIN_WATER)
    {
        gBattle_BG1_Y = Cos2(gTasks[taskId].data[6]) / 512 - 8;
        if (gTasks[taskId].data[6] < 180)
            gTasks[taskId].data[6] += 4;
        else
            gTasks[taskId].data[6] += 6;

        if (gTasks[taskId].data[6] == 360)
            gTasks[taskId].data[6] = 0;
    }

    switch (gTasks[taskId].tState)
    {
    case 0:
        gTasks[taskId].data[4] = 16;
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            gTasks[taskId].data[2] = 16;
            gTasks[taskId].tState++;
        }
        else
        {
            gTasks[taskId].data[2] = 1;
            gTasks[taskId].tState++;
        }
        break;
    case 1:
        if (--gTasks[taskId].data[2] == 0)
        {
            gTasks[taskId].tState++;
            SetGpuWindowIn(WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR);
        }
        break;
    case 2:
        gTasks[taskId].data[7] += 1;
        gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        if (gTasks[taskId].data[7] >= 0x30)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].data[2] = DisplayWidth();
            gTasks[taskId].data[3] = 32;
            gTasks[taskId].data[5] = 1;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].data[3])
        {
            if (--gTasks[taskId].data[3] == 0)
            {
                SetGpuState(GPU_STATE_BLDCNT, BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ);
                SetGpuState(GPU_STATE_BLDALPHA, BLDALPHA_BLEND(15, 0));
                SetGpuState(GPU_STATE_BLDY, 0);
            }
        }
        else
        {
            if ((gTasks[taskId].data[4] & 0x1F) && --gTasks[taskId].data[5] == 0)
            {
                gTasks[taskId].data[4] += 0xFF;
                gTasks[taskId].data[5] = 4;
            }
        }

        if (gTasks[taskId].data[7] < DisplayHeight() / 2)
        {
            gTasks[taskId].data[7] += 4;
            gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        }
        else
        {
            gBattle_WIN0V = WIN_RANGE(0, DisplayHeight() + 1);
        }

        if (gTasks[taskId].data[2])
            gTasks[taskId].data[2] -= 2;

        // Scanline settings have already been set in CB2_InitBattleInternal()
        for (i = 0; i < DisplayHeight() / 2; i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = gTasks[taskId].data[2];

        for (; i < DisplayHeight(); i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = -gTasks[taskId].data[2];

        if (gTasks[taskId].data[2] == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
            CpuFill32(0, (void *)BG_SCREEN_ADDR(28), BG_SCREEN_SIZE);
            SetBgAttribute(1, BG_ATTR_CHARBASEINDEX, 0);
            SetBgAttribute(2, BG_ATTR_CHARBASEINDEX, 0);

            ClearGpuBackgroundState(1);
            SetGpuBackgroundScreenBaseBlock(1, 28);
            SetGpuBackgroundWidth(1, 256);
            SetGpuBackgroundHeight(1, 512);

            ClearGpuBackgroundState(2);
            SetGpuBackgroundScreenBaseBlock(2, 30);
            SetGpuBackgroundWidth(2, 512);
            SetGpuBackgroundHeight(2, 256);
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }

    if (gTasks[taskId].tState != 4)
        SetGpuState(GPU_STATE_BLDALPHA, BLDALPHA_BLEND(gTasks[taskId].data[4], 0));
}

static void BattleIntroSlide3(u8 taskId)
{
    int i;

    gBattle_BG1_X += 8;
    switch (gTasks[taskId].tState)
    {
    case 0:
        SetGpuState(GPU_STATE_BLDCNT, BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ);
        SetGpuState(GPU_STATE_BLDALPHA, BLDALPHA_BLEND(8, 8));
        SetGpuState(GPU_STATE_BLDY, 0);
        gTasks[taskId].data[4] = BLDALPHA_BLEND(8, 8);
        if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK))
        {
            gTasks[taskId].data[2] = 16;
            gTasks[taskId].tState++;
        }
        else
        {
            gTasks[taskId].data[2] = 1;
            gTasks[taskId].tState++;
        }
        break;
    case 1:
        if (--gTasks[taskId].data[2] == 0)
        {
            gTasks[taskId].tState++;
            SetGpuWindowIn(WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR);
        }
        break;
    case 2:
        gTasks[taskId].data[7] += 1;
        gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        if (gTasks[taskId].data[7] >= 0x30)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].data[2] = DisplayWidth();
            gTasks[taskId].data[3] = 32;
            gTasks[taskId].data[5] = 1;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].data[3])
        {
            gTasks[taskId].data[3]--;
        }
        else
        {
            if ((gTasks[taskId].data[4] & 0xF) && --gTasks[taskId].data[5] == 0)
            {
                gTasks[taskId].data[4] += 0xFF;
                gTasks[taskId].data[5] = 6;
            }
        }

        if (gTasks[taskId].data[7] < DisplayHeight() / 2)
        {
            gTasks[taskId].data[7] += 4;
            gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        }
        else
        {
            gBattle_WIN0V = WIN_RANGE(0, DisplayHeight() + 1);
        }

        if (gTasks[taskId].data[2])
            gTasks[taskId].data[2] -= 2;

        // Scanline settings have already been set in CB2_InitBattleInternal()
        for (i = 0; i < DisplayHeight() / 2; i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = gTasks[taskId].data[2];

        for (; i < DisplayHeight(); i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = -gTasks[taskId].data[2];

        if (gTasks[taskId].data[2] == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
            CpuFill32(0, (void *)BG_SCREEN_ADDR(28), BG_SCREEN_SIZE);
            SetBgAttribute(1, BG_ATTR_CHARBASEINDEX, 0);
            SetBgAttribute(2, BG_ATTR_CHARBASEINDEX, 0);

            ClearGpuBackgroundState(1);
            SetGpuBackgroundScreenBaseBlock(1, 28);
            SetGpuBackgroundWidth(1, 256);
            SetGpuBackgroundHeight(1, 512);

            ClearGpuBackgroundState(2);
            SetGpuBackgroundScreenBaseBlock(2, 30);
            SetGpuBackgroundWidth(2, 512);
            SetGpuBackgroundHeight(2, 256);
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }

    if (gTasks[taskId].tState != 4)
        SetGpuState(GPU_STATE_BLDALPHA, BLDALPHA_BLEND(gTasks[taskId].data[4], 0));
}

static void BattleIntroSlideLink(u8 taskId)
{
    int i;

    if (gTasks[taskId].tState > 1 && !gTasks[taskId].data[4])
    {
        u16 var0 = gBattle_BG1_X & 0x8000;
        if (var0 || gBattle_BG1_X < 80)
        {
            gBattle_BG1_X += 3;
            gBattle_BG2_X -= 3;
        }
        else
        {
            CpuFill32(0, (void *)BG_SCREEN_ADDR(28), BG_SCREEN_SIZE);
            CpuFill32(0, (void *)BG_SCREEN_ADDR(30), BG_SCREEN_SIZE);
            gTasks[taskId].data[4] = 1;
        }
    }

    switch (gTasks[taskId].tState)
    {
    case 0:
        gTasks[taskId].data[2] = 32;
        gTasks[taskId].tState++;
        break;
    case 1:
        if (--gTasks[taskId].data[2] == 0)
        {
            gTasks[taskId].tState++;
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].oam.objMode = ST_OAM_OBJ_WINDOW;
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].callback = SpriteCB_VsLetterInit;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].oam.objMode = ST_OAM_OBJ_WINDOW;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].callback = SpriteCB_VsLetterInit;
            SetGpuWindowIn(WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR);
            SetGpuWindowOut(WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2);
        }
        break;
    case 2:
        gTasks[taskId].data[7] += 1;
        gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        if (gTasks[taskId].data[7] >= 0x30)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].data[2] = DisplayWidth();
            gTasks[taskId].data[3] = 32;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].data[7] < DisplayHeight() / 2)
        {
            gTasks[taskId].data[7] += 4;
            gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        }
        else
        {
            gBattle_WIN0V = WIN_RANGE(0, DisplayHeight() + 1);
        }

        if (gTasks[taskId].data[2])
            gTasks[taskId].data[2] -= 2;

        // Scanline settings have already been set in CB2_InitBattleInternal()
        for (i = 0; i < DisplayHeight() / 2; i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = gTasks[taskId].data[2];

        for (; i < DisplayHeight(); i++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = -gTasks[taskId].data[2];

        if (gTasks[taskId].data[2] == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;

            SetBgAttribute(1, BG_ATTR_CHARBASEINDEX, 0);
            SetBgAttribute(2, BG_ATTR_CHARBASEINDEX, 0);

            ClearGpuBackgroundState(1);
            SetGpuBackgroundScreenBaseBlock(1, 28);
            SetGpuBackgroundWidth(1, 256);
            SetGpuBackgroundHeight(1, 512);

            ClearGpuBackgroundState(2);
            SetGpuBackgroundScreenBaseBlock(2, 30);
            SetGpuBackgroundWidth(2, 512);
            SetGpuBackgroundHeight(2, 256);
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }
}

static void BattleIntroSlidePartner(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
    case 0:
        gTasks[taskId].data[2] = 1;
        gTasks[taskId].tState++;
        break;
    case 1:
        if (--gTasks[taskId].data[2] == 0)
        {
            gTasks[taskId].tState++;

            ClearGpuBackgroundState(1);
            SetGpuBackgroundPriority(1, 2);
            SetGpuBackgroundCharBaseBlock(1, 2);
            SetGpuBackgroundScreenBaseBlock(1, 28);
            SetGpuBackgroundWidth(1, 512);
            SetGpuBackgroundHeight(1, 256);

            ClearGpuBackgroundState(2);
            SetGpuBackgroundPriority(2, 2);
            SetGpuBackgroundCharBaseBlock(2, 2);
            SetGpuBackgroundScreenBaseBlock(2, 30);
            SetGpuBackgroundWidth(2, 512);
            SetGpuBackgroundHeight(2, 256);

            SetGpuState(GPU_STATE_DISPCNT, GetGpuState(GPU_STATE_DISPCNT) | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON | DISPCNT_OBJWIN_ON);
            SetGpuWindowIn(WININ_WIN1_BG1 | WININ_WIN1_BG2 | WININ_WIN1_BG3 | WININ_WIN1_OBJ | WININ_WIN1_CLR);
            SetGpuWindowOut(WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR);
            gBattle_BG0_Y = -48;
            gBattle_BG1_X = DisplayWidth();
            gBattle_BG2_X = -DisplayWidth();
        }
        break;
    case 2:
        gBattle_WIN0V += 0x1 << 16;
        if ((gBattle_WIN0V >> 16) != 0x1)
            gBattle_WIN0V--;

        if ((gBattle_WIN0V >> 16) == 0x20)
        {
            gTasks[taskId].data[7] = 0x20;
            gTasks[taskId].tState++;
            gTasks[taskId].data[2] = DisplayWidth();
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].data[7] < (DisplayHeight() / 2) - 4)
        {
            gTasks[taskId].data[7] += 4;
            gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        }

        if (gTasks[taskId].data[2])
            gTasks[taskId].data[2] -= 2;

        gBattle_BG1_X = gTasks[taskId].data[2];
        gBattle_BG2_X = -gTasks[taskId].data[2];
        if (gTasks[taskId].data[2] == 0)
            gTasks[taskId].tState++;
        break;
    case 4:
        gBattle_BG0_Y += 2;
        gBattle_BG2_Y += 2;
        if (gTasks[taskId].data[7] < DisplayHeight() / 2)
        {
            gTasks[taskId].data[7] += 4;
            gBattle_WIN0V = WIN_RANGE((DisplayHeight() / 2) - gTasks[taskId].data[7], ((DisplayHeight() / 2) + 1) + gTasks[taskId].data[7]);
        }
        else
        {
            gBattle_WIN0V = WIN_RANGE(0, DisplayHeight() + 1);
        }

        if (!gBattle_BG0_Y)
        {
            CpuFill32(0, (void *)BG_SCREEN_ADDR(28), BG_SCREEN_SIZE * 4);
            SetGpuState(GPU_STATE_DISPCNT, GetGpuState(GPU_STATE_DISPCNT) & ~DISPCNT_WIN1_ON);
            SetBgAttribute(1, BG_ATTR_CHARBASEINDEX, 0);
            SetBgAttribute(2, BG_ATTR_CHARBASEINDEX, 0);
            ClearGpuBackgroundState(1);
            SetGpuBackgroundPriority(1, 0);
            SetGpuBackgroundCharBaseBlock(1, 0);
            SetGpuBackgroundScreenBaseBlock(1, 28);
            ClearGpuBackgroundState(2);
            SetGpuBackgroundPriority(2, 0);
            SetGpuBackgroundCharBaseBlock(2, 0);
            SetGpuBackgroundScreenBaseBlock(2, 30);
            SetGpuBackgroundWidth(2, 512);
            SetGpuBackgroundHeight(2, 256);
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
        }
        break;
    case 5:
        BattleIntroSlideEnd(taskId);
        break;
    }
}

void DrawBattlerOnBg(int bgId, u8 x, u8 y, u8 battlerPosition, u8 paletteId, u8 *tiles, u16 *tilemap, u16 tilesOffset)
{
    int i, j;
    u8 battler = GetBattlerAtPosition(battlerPosition);
    int offset = tilesOffset;
    CpuCopy16(gMonSpritesGfxPtr->sprites.ptr[battlerPosition] + 0x800 * gBattleMonForms[battler], tiles, 0x800);
    LoadBgTiles(bgId, tiles, 0x1000, tilesOffset);
    for (i = y; i < y + 8; i++)
    {
        for (j = x; j < x + 8; j++)
        {
            tilemap[i * 32 + j] = offset | (paletteId << 12);
            offset++;
        }
    }
    LoadBgTilemap(bgId, tilemap, 0x800, 0);
}

static void UNUSED DrawBattlerOnBgDMA(u8 x, u8 y, u8 battlerPosition, u8 arg3, u8 paletteId, u16 arg5, u8 arg6, u8 arg7)
{
    int i, j, offset;

    DmaCopy16(3, gMonSpritesGfxPtr->sprites.ptr[battlerPosition] + 0x800 * arg3, (void *)BG_SCREEN_ADDR(0) + arg5, 0x800);
    offset = (arg5 >> 5) - (arg7 << 9);
    for (i = y; i < y + 8; i++)
    {
        for (j = x; j < x + 8; j++)
        {
            *((u16 *)(gpu.tileMaps) + (i * 32) + (j + (arg6 << 10))) = offset | (paletteId << 12);
            offset++;
        }
    }
}
