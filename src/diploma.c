#include "global.h"
#include "diploma.h"
#include "palette.h"
#include "main.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "task.h"
#include "malloc.h"
#include "decompress.h"
#include "bg.h"
#include "window.h"
#include "string_util.h"
#include "text.h"
#include "overworld.h"
#include "menu.h"
#include "pokedex.h"
#include "constants/rgb.h"
#include "gpu_main.h"

extern const u8 gText_DexNational[];
extern const u8 gText_DexHoenn[];
extern const u8 gText_PokedexDiploma[];

static void MainCB2(void);
static void Task_DiplomaFadeIn(u8);
static void Task_DiplomaWaitForKeyPress(u8);
static void Task_DiplomaFadeOut(u8);
static void DisplayDiplomaText(void);
static void InitDiplomaBg(void);
static void InitDiplomaWindow(void);
static void PrintDiplomaText(u8 *, u8, u8);

EWRAM_DATA static u8 *sDiplomaTilemapPtr = NULL;

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static const u16 sDiplomaPalettes[][16] =
{
    INCBIN_U16("graphics/diploma/national.gbapal"),
    INCBIN_U16("graphics/diploma/hoenn.gbapal"),
};

static const u32 sDiplomaTilemap[] = INCBIN_U32("graphics/diploma/tilemap.bin.lz");
static const u32 sDiplomaTiles[] = INCBIN_U32("graphics/diploma/tiles.4bpp.lz");

void CB2_ShowDiploma(void)
{
    SetVBlankCallback(NULL);
    SetGpuState(GPU_STATE_DISPCNT, DISPCNT_MODE_0);
    ClearGpuBackgroundState(3);
    ClearGpuBackgroundState(2);
    ClearGpuBackgroundState(1);
    ClearGpuBackgroundState(0);
    SetGpuBackgroundX(3, 0);
    SetGpuBackgroundY(3, 0);
    SetGpuBackgroundX(2, 0);
    SetGpuBackgroundY(2, 0);
    SetGpuBackgroundX(1, 0);
    SetGpuBackgroundY(1, 0);
    SetGpuBackgroundX(0, 0);
    SetGpuBackgroundY(0, 0);
    GpuClearAll();
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    LoadPalette(sDiplomaPalettes, BG_PLTT_ID(0), sizeof(sDiplomaPalettes));
    sDiplomaTilemapPtr = Alloc(0x1000);
    InitDiplomaBg();
    InitDiplomaWindow();
    ResetTempTileDataBuffers();
    DecompressAndCopyTileDataToVram(1, &sDiplomaTiles, 0, 0, 0);
    while (FreeTempTileDataBuffersIfPossible())
        ;
    LZDecompressWram(sDiplomaTilemap, sDiplomaTilemapPtr);
    CopyBgTilemapBufferToVram(1);
    DisplayDiplomaText();
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    EnableInterrupts(1);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB2);
    CreateTask(Task_DiplomaFadeIn, 0);
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void Task_DiplomaFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_DiplomaWaitForKeyPress;
}

static void Task_DiplomaWaitForKeyPress(u8 taskId)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_DiplomaFadeOut;
    }
}

static void Task_DiplomaFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        Free(sDiplomaTilemapPtr);
        FreeAllWindowBuffers();
        DestroyTask(taskId);
        SetMainCallback2(CB2_ReturnToFieldFadeFromBlack);
    }
}

static void DisplayDiplomaText(void)
{
    if (HasAllMons())
    {
        SetGpuBackgroundX(1, DisplayWidth() + 16);
        StringCopy(gStringVar1, gText_DexNational);
    }
    else
    {
        SetGpuBackgroundX(1, 0);
        StringCopy(gStringVar1, gText_DexHoenn);
    }
    StringExpandPlaceholders(gStringVar4, gText_PokedexDiploma);
    PrintDiplomaText(gStringVar4, 0, 1);
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}

static const struct BgTemplate sDiplomaBgTemplates[2] =
{
    {
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 31,
        .screenWidth = 256,
        .screenHeight = 256,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 6,
        .screenWidth = 512,
        .screenHeight = 256,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
};

static void InitDiplomaBg(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sDiplomaBgTemplates, ARRAY_COUNT(sDiplomaBgTemplates));
    SetBgTilemapBuffer(1, sDiplomaTilemapPtr);
    SetGpuState(GPU_STATE_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP | DISPCNT_GBA_MODE);
    ShowBg(0);
    ShowBg(1);
    SetGpuState(GPU_STATE_BLDCNT, 0);
    SetGpuState(GPU_STATE_BLDALPHA, 0);
    SetGpuState(GPU_STATE_BLDY, 0);
}

static const struct WindowTemplate sDiplomaWinTemplates[2] =
{
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 2,
        .width = 20,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    DUMMY_WIN_TEMPLATE,
};

static void InitDiplomaWindow(void)
{
    InitWindows(sDiplomaWinTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    FillWindowPixelBuffer(0, PIXEL_FILL(0));
    PutWindowTilemap(0);
}

static void PrintDiplomaText(u8 *text, u8 var1, u8 var2)
{
    u8 color[3] = {0, 2, 3};

    AddTextPrinterParameterized4(0, FONT_NORMAL, var1, var2, 0, 0, color, TEXT_SKIP_DRAW, text);
}
