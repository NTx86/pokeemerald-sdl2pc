#include "global.h"
#include "braille_puzzles.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "fldeff.h"
#include "gpu_regs.h"
#include "main.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "constants/songs.h"
#include "constants/map_types.h"
#include "gpu_main.h"

struct FlashStruct
{
    u8 fromType;
    u8 toType;
    bool8 isEnter;
    bool8 isExit;
    void (*func)(void);
};

static void FieldCallback_Flash(void);
static void FldEff_UseFlash(void);
static bool8 TryDoMapTransition(void);
static void DoExitCaveTransition(void);
static void Task_ExitCaveTransition1(u8 taskId);
static void Task_ExitCaveTransition2(u8 taskId);
static void Task_ExitCaveTransition3(u8 taskId);
static void Task_ExitCaveTransition4(u8 taskId);
static void Task_ExitCaveTransition5(u8 taskId);
static void DoEnterCaveTransition(void);
static void Task_EnterCaveTransition1(u8 taskId);
static void Task_EnterCaveTransition2(u8 taskId);
static void Task_EnterCaveTransition3(u8 taskId);
static void Task_EnterCaveTransition4(u8 taskId);

static const struct FlashStruct sTransitionTypes[] =
{
    {MAP_TYPE_TOWN,        MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_CITY,        MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_ROUTE,       MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_UNDERWATER,  MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_OCEAN_ROUTE, MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_UNKNOWN,     MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_INDOOR,      MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_SECRET_BASE, MAP_TYPE_UNDERGROUND,  TRUE, FALSE, DoEnterCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_TOWN,        FALSE,  TRUE, DoExitCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_CITY,        FALSE,  TRUE, DoExitCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_ROUTE,       FALSE,  TRUE, DoExitCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_UNDERWATER,  FALSE,  TRUE, DoExitCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_OCEAN_ROUTE, FALSE,  TRUE, DoExitCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_UNKNOWN,     FALSE,  TRUE, DoExitCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_INDOOR,      FALSE,  TRUE, DoExitCaveTransition},
    {MAP_TYPE_UNDERGROUND, MAP_TYPE_SECRET_BASE, FALSE,  TRUE, DoExitCaveTransition},
    {},
};

static const u16 sCaveTransitionPalette_White[] = INCGFX_U16("graphics/cave_transition/white.pal", ".gbapal");
static const u16 sCaveTransitionPalette_Black[] = INCGFX_U16("graphics/cave_transition/black.pal", ".gbapal");

static const u16 sCaveTransitionPalette_Enter[] = INCGFX_U16("graphics/cave_transition/enter.pal", ".gbapal");

static const u32 sCaveTransitionTilemap[] = INCGFX_U32("graphics/cave_transition/tilemap.bin", ".lz");
static const u32 sCaveTransitionTiles[] = INCGFX_U32("graphics/cave_transition/tiles.png", ".4bpp.lz");

bool8 SetUpFieldMove_Flash(void)
{
    // In Ruby and Sapphire, Registeel's tomb is opened by using Fly. In Emerald,
    // Flash is used instead.
    if (ShouldDoBrailleRegisteelEffect())
    {
        gSpecialVar_Result = GetCursorSelectionMonId();
        gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = SetUpPuzzleEffectRegisteel;
        return TRUE;
    }
    else if (gMapHeader.cave == TRUE && !FlagGet(FLAG_SYS_USE_FLASH))
    {
        gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_Flash;
        return TRUE;
    }

    return FALSE;
}

static void FieldCallback_Flash(void)
{
    u8 taskId = CreateFieldMoveTask();
    gFieldEffectArguments[0] = GetCursorSelectionMonId();
    gTasks[taskId].ptr.funcPtr = FldEff_UseFlash;
}

static void FldEff_UseFlash(void)
{
    PlaySE(SE_M_REFLECT);
    FlagSet(FLAG_SYS_USE_FLASH);
    ScriptContext_SetupScript(EventScript_UseFlash);
}

static void CB2_ChangeMapMain(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBC_ChangeMapVBlank(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_DoChangeMap(void)
{
    SetVBlankCallback(NULL);
    ResetGpuDisplayControl();
    ClearGpuBackgroundState(2);
    ClearGpuBackgroundState(1);
    ClearGpuBackgroundState(1);
    ClearGpuBackgroundState(0);
    SetGpuBackgroundX(2, 0);
    SetGpuBackgroundY(2, 0);
    SetGpuBackgroundX(1, 0);
    SetGpuBackgroundY(1, 0);
    SetGpuBackgroundX(0, 0);
    SetGpuBackgroundY(0, 0);
    GpuClearData();
    GpuClearSprites();
    GpuClearPalette2();
    ResetPaletteFade();
    ResetTasks();
    ResetSpriteData();
    IntrEnable(INTR_FLAG_VBLANK);
    SetVBlankCallback(VBC_ChangeMapVBlank);
    SetMainCallback2(CB2_ChangeMapMain);
    if (!TryDoMapTransition())
        SetMainCallback2(gMain.savedCallback);
}

static bool8 TryDoMapTransition(void)
{
    u8 i;
    u8 fromType = GetLastUsedWarpMapType();
    u8 toType = GetCurrentMapType();

    for (i = 0; sTransitionTypes[i].fromType; i++)
    {
        if (sTransitionTypes[i].fromType == fromType && sTransitionTypes[i].toType == toType)
        {
            sTransitionTypes[i].func();
            return TRUE;
        }
    }

    return FALSE;
}

bool8 GetMapPairFadeToType(u8 _fromType, u8 _toType)
{
    u8 i;
    u8 fromType = _fromType;
    u8 toType = _toType;

    for (i = 0; sTransitionTypes[i].fromType; i++)
    {
        if (sTransitionTypes[i].fromType == fromType && sTransitionTypes[i].toType == toType)
        {
            return sTransitionTypes[i].isEnter;
        }
    }

    return FALSE;
}

bool8 GetMapPairFadeFromType(u8 _fromType, u8 _toType)
{
    u8 i;
    u8 fromType = _fromType;
    u8 toType = _toType;

    for (i = 0; sTransitionTypes[i].fromType; i++)
    {
        if (sTransitionTypes[i].fromType == fromType && sTransitionTypes[i].toType == toType)
        {
            return sTransitionTypes[i].isExit;
        }
    }

    return FALSE;
}

static void DoExitCaveTransition(void)
{
    CreateTask(Task_ExitCaveTransition1, 0);
}

static void Task_ExitCaveTransition1(u8 taskId)
{
    gTasks[taskId].func = Task_ExitCaveTransition2;
}

static void Task_ExitCaveTransition2(u8 taskId)
{
    ResetGpuDisplayControl();
    LZ77UnCompVram(sCaveTransitionTiles, (void *)(gpu.gfxData + (BG_CHAR_SIZE * 3)));
    LZ77UnCompVram(sCaveTransitionTilemap, (void *)(gpu.tileMaps + (BG_SCREEN_SIZE * 0x1F)));
    LoadPalette(sCaveTransitionPalette_White, BG_PLTT_ID(14), PLTT_SIZE_4BPP);
    LoadPalette(&sCaveTransitionPalette_Enter[8], BG_PLTT_ID(14), PLTT_SIZEOF(8));
    SetGpuState(GPU_STATE_BLDCNT, BLDCNT_TGT1_BG0
                                | BLDCNT_EFFECT_BLEND
                                | BLDCNT_TGT2_BG1
                                | BLDCNT_TGT2_BG2
                                | BLDCNT_TGT2_BG3
                                | BLDCNT_TGT2_OBJ
                                | BLDCNT_TGT2_BD);
    SetGpuState(GPU_STATE_BLDALPHA, 0);
    SetGpuState(GPU_STATE_BLDY, 0);
    ClearGpuBackgroundState(0);
    SetGpuBackgroundCharBaseBlock(0, 3);
    SetGpuBackgroundScreenBaseBlock(0, 31);
    SetGpuState(GPU_STATE_DISPCNT, DISPCNT_MODE_0
                                | DISPCNT_OBJ_1D_MAP
                                | DISPCNT_BG0_ON
                                | DISPCNT_OBJ_ON
                                | DISPCNT_GBA_MODE);
    gTasks[taskId].func = Task_ExitCaveTransition3;
    gTasks[taskId].data[0] = 16;
    gTasks[taskId].data[1] = 0;
}

static void Task_ExitCaveTransition3(u8 taskId)
{
    u16 count = gTasks[taskId].data[1];
    u16 blend = count + 0x1000;

    SetGpuState(GPU_STATE_BLDALPHA, blend);
    if (count <= 16)
    {
        gTasks[taskId].data[1]++;
    }
    else
    {
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].func = Task_ExitCaveTransition4;
    }
}

static void Task_ExitCaveTransition4(u8 taskId)
{
    u16 count;

    SetGpuState(GPU_STATE_BLDALPHA, BLDALPHA_BLEND(16, 16));
    count = gTasks[taskId].data[2];

    if (count < 8)
    {
        gTasks[taskId].data[2]++;
        LoadPalette(&sCaveTransitionPalette_Enter[8 + count], BG_PLTT_ID(14), PLTT_SIZEOF(8) - PLTT_SIZEOF(count));
    }
    else
    {
        LoadPalette(sCaveTransitionPalette_White, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        gTasks[taskId].func = Task_ExitCaveTransition5;
        gTasks[taskId].data[2] = 8;
    }
}

static void Task_ExitCaveTransition5(u8 taskId)
{
    if (gTasks[taskId].data[2])
        gTasks[taskId].data[2]--;
    else
        SetMainCallback2(gMain.savedCallback);
}

static void DoEnterCaveTransition(void)
{
    CreateTask(Task_EnterCaveTransition1, 0);
}

static void Task_EnterCaveTransition1(u8 taskId)
{
    gTasks[taskId].func = Task_EnterCaveTransition2;
}

static void Task_EnterCaveTransition2(u8 taskId)
{
    ResetGpuDisplayControl();
    LZ77UnCompVram(sCaveTransitionTiles, (void *)(gpu.gfxData + (BG_CHAR_SIZE * 3)));
    LZ77UnCompVram(sCaveTransitionTilemap, (void *)(gpu.tileMaps + (BG_SCREEN_SIZE * 0x1F)));
    SetGpuState(GPU_STATE_BLDCNT, 0);
    SetGpuState(GPU_STATE_BLDALPHA, 0);
    SetGpuState(GPU_STATE_BLDY, 0);
    ClearGpuBackgroundState(0);
    SetGpuBackgroundCharBaseBlock(0, 3);
    SetGpuBackgroundScreenBaseBlock(0, 31);
    SetGpuState(GPU_STATE_DISPCNT, DISPCNT_MODE_0
                                | DISPCNT_OBJ_1D_MAP
                                | DISPCNT_BG0_ON
                                | DISPCNT_OBJ_ON
                                | DISPCNT_GBA_MODE);
    LoadPalette(sCaveTransitionPalette_White, BG_PLTT_ID(14), PLTT_SIZE_4BPP);
    LoadPalette(sCaveTransitionPalette_Black, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
    gTasks[taskId].func = Task_EnterCaveTransition3;
    gTasks[taskId].data[0] = 16;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].data[2] = 0;
}

static void Task_EnterCaveTransition3(u8 taskId)
{
    u16 count = gTasks[taskId].data[2];

    if (count < 16)
    {
        gTasks[taskId].data[2]++;
        gTasks[taskId].data[2]++;
        LoadPalette(&sCaveTransitionPalette_Enter[15 - count], BG_PLTT_ID(14), PLTT_SIZEOF(count + 1));
    }
    else
    {
        SetGpuState(GPU_STATE_BLDALPHA, BLDALPHA_BLEND(16, 16));
        SetGpuState(GPU_STATE_BLDCNT, BLDCNT_TGT1_BG0
                                    | BLDCNT_EFFECT_BLEND
                                    | BLDCNT_TGT2_BG1
                                    | BLDCNT_TGT2_BG2
                                    | BLDCNT_TGT2_BG3
                                    | BLDCNT_TGT2_OBJ
                                    | BLDCNT_TGT2_BD);
        gTasks[taskId].func = Task_EnterCaveTransition4;
    }
}

static void Task_EnterCaveTransition4(u8 taskId)
{
    u16 count = 16 - gTasks[taskId].data[1];
    u16 blend = count + 0x1000;

    SetGpuState(GPU_STATE_BLDALPHA, blend);
    if (count)
    {
        gTasks[taskId].data[1]++;
    }
    else
    {
        LoadPalette(sCaveTransitionPalette_Black, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        SetMainCallback2(gMain.savedCallback);
    }
}
