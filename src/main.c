#include "global.h"
#include "crt0.h"
#include "malloc.h"
#include "link.h"
#include "link_rfu.h"
#include "librfu.h"
#include "m4a.h"
#include "bg.h"
#include "rtc.h"
#include "scanline_effect.h"
#include "overworld.h"
#include "play_time.h"
#include "random.h"
#include "dma3.h"
#include "gba/flash_internal.h"
#include "load_save.h"
#include "gpu_regs.h"
#include "agb_flash.h"
#include "sound.h"
#include "battle.h"
#include "battle_controllers.h"
#include "text.h"
#include "intro.h"
#include "main.h"
#include "trainer_hill.h"
#include "platform.h"
#include "constants/rgb.h"
#include "gpu_main.h"

static void SerialIntr(void);

const u8 gGameVersion = GAME_VERSION;

const u8 gGameLanguage = GAME_LANGUAGE; // English

const char BuildDateTime[] = "2005 02 21 11:10";

#ifndef PORTABLE
static u16 sUnusedVar; // Never read

u8 gHeap[HEAP_SIZE];
#endif

u16 gKeyRepeatStartDelay;
bool8 gLinkTransferringData;
struct Main gMain;
u16 gKeyRepeatContinueDelay;
bool8 gSoftResetDisabled;
u8 gLinkVSyncDisabled;
u32 IntrMain_Buffer[0x200];
s8 gPcmDmaCounter;

static EWRAM_DATA u16 sTrainerId = 0;

//EWRAM_DATA void (**gFlashTimerIntrFunc)(void) = NULL;

static void UpdateLinkAndCallCallbacks(void);
static void InitMainCallbacks(void);
static void CallCallbacks(void);
#ifdef BUGFIX
static void SeedRngWithRtc(void);
#endif
static void ReadKeys(void);
void InitIntrHandlers(void);

struct GameBorder borderList[NUM_GAME_BORDERS] = {
    { "emerald",      -1, BORDER_REPEAT_X | BORDER_REPEAT_Y | BORDER_CLAMP_Y },
    { "emerald-menu", -1, BORDER_REPEAT_X | BORDER_REPEAT_Y },
    { "battle",        2, BORDER_REPEAT_X | BORDER_REPEAT_Y | BORDER_CLAMP_Y },
    { "intro1",       -1, BORDER_REPEAT_X | BORDER_REPEAT_Y | BORDER_CLAMP_Y },
    { "intro2",       -1, BORDER_REPEAT_X | BORDER_REPEAT_Y | BORDER_CLAMP_Y },
    { "intro3",       -1, BORDER_REPEAT_X | BORDER_REPEAT_Y },
    { "intro4",       -1, BORDER_REPEAT_X | BORDER_REPEAT_Y },
    { "intro5",       -1, BORDER_REPEAT_X | BORDER_REPEAT_Y | BORDER_CLAMP_X | BORDER_CLAMP_Y },
    { "overlay",      -1, 0 },
};

#define B_START_SELECT (B_BUTTON | START_BUTTON | SELECT_BUTTON)

void GameInit(void)
{
    GpuInit();
    *(u16 *)BG_PLTT = RGB_WHITE; // Set the backdrop to white on startup
    InitGpuRegManager();
    REG_WAITCNT = WAITCNT_PREFETCH_ENABLE | WAITCNT_WS0_S_1 | WAITCNT_WS0_N_3;
    InitKeys();
    InitIntrHandlers();
    m4aSoundInit();
#ifndef PORTABLE
    InitRFU();
#endif
    RtcInit();
    CheckForFlashMemory();
    InitMainCallbacks();
    InitMapMusic();
#ifdef BUGFIX
    SeedRngWithRtc(); // see comment at SeedRngWithRtc definition below
#endif
    ResetBgs();
    SetDefaultFontsPointer();

    InitHeap();

    gSoftResetDisabled = FALSE;

#ifndef PORTABLE
    if (gFlashMemoryPresent != TRUE)
        SetMainCallback2(NULL);
#endif

    gLinkTransferringData = FALSE;

#ifndef PORTABLE
    sUnusedVar = 0xFC0;
#endif

#ifndef NDEBUG
#if (LOG_HANDLER == LOG_HANDLER_MGBA_PRINT)
    (void) MgbaOpen();
#elif (LOG_HANDLER == LOG_HANDLER_AGB_PRINT)
    AGBPrintfInit();
#endif
#endif
}

void GameLoop(void)
{
    ReadKeys();

    if (gSoftResetDisabled == FALSE
     && JOY_HELD_RAW(A_BUTTON)
     && JOY_HELD_RAW(B_START_SELECT) == B_START_SELECT)
    {
        rfu_REQ_stopMode();
        rfu_waitREQComplete();
        DoSoftReset();
    }

    if (Overworld_SendKeysToLinkIsRunning() == TRUE)
    {
        gLinkTransferringData = TRUE;
        UpdateLinkAndCallCallbacks();
        gLinkTransferringData = FALSE;
    }
    else
    {
        gLinkTransferringData = FALSE;
        UpdateLinkAndCallCallbacks();

        if (Overworld_RecvKeysFromLinkIsRunning() == TRUE)
        {
            gMain.newKeys = 0;
            gLinkTransferringData = TRUE;
            UpdateLinkAndCallCallbacks();
            gLinkTransferringData = FALSE;
        }
    }

    PlayTimeCounter_Update();
    MapMusicMain();
    VBlankIntrWait();
}

static void UpdateLinkAndCallCallbacks(void)
{
    if (!HandleLinkConnection())
        CallCallbacks();
}

static void InitMainCallbacks(void)
{
    gMain.vblankCounter1 = 0;
    gTrainerHillVBlankCounter = NULL;
    gMain.vblankCounter2 = 0;
    gMain.callback1 = NULL;
    SetMainCallback2(CB2_InitCopyrightScreenAfterBootup);
    gSaveBlock2Ptr = &gSaveblock2.block;
    gPokemonStoragePtr = &gPokemonStorage.block;
}

static void CallCallbacks(void)
{
    if (gMain.callback1)
        gMain.callback1();

    if (gMain.callback2)
        gMain.callback2();
}

void SetMainCallback2(MainCallback callback)
{
    gMain.callback2 = callback;
    gMain.state = 0;
}

void StartTimer1(void)
{
    REG_TM1CNT_H = 0x80;
}

void SeedRngAndSetTrainerId(void)
{
    u16 val = REG_TM1CNT_L;
    SeedRng(val);
    REG_TM1CNT_H = 0;
    sTrainerId = val;
}

u16 GetGeneratedTrainerIdLower(void)
{
    return sTrainerId;
}

// FRLG commented this out to remove RTC, however Emerald didn't undo this!
#ifdef BUGFIX
static void SeedRngWithRtc(void)
{
    u32 seed = RtcGetMinuteCount();
    seed = (seed >> 16) ^ (seed & 0xFFFF);
    SeedRng(seed);
}
#endif

void InitKeys(void)
{
    gKeyRepeatContinueDelay = 5;
    gKeyRepeatStartDelay = 40;

    gMain.heldKeys = 0;
    gMain.newKeys = 0;
    gMain.newAndRepeatedKeys = 0;
    gMain.heldKeysRaw = 0;
    gMain.newKeysRaw = 0;
}

static void ReadKeys(void)
{
    u16 keyInput = Platform_GetKeyInput();

    gMain.newKeysRaw = keyInput & ~gMain.heldKeysRaw;
    gMain.newKeys = gMain.newKeysRaw;
    gMain.newAndRepeatedKeys = gMain.newKeysRaw;

    // BUG: Key repeat won't work when pressing L using L=A button mode
    // because it compares the raw key input with the remapped held keys.
    // Note that newAndRepeatedKeys is never remapped either.

    if (keyInput != 0 && gMain.heldKeys == keyInput)
    {
        gMain.keyRepeatCounter--;

        if (gMain.keyRepeatCounter == 0)
        {
            gMain.newAndRepeatedKeys = keyInput;
            gMain.keyRepeatCounter = gKeyRepeatContinueDelay;
        }
    }
    else
    {
        // If there is no input or the input has changed, reset the counter.
        gMain.keyRepeatCounter = gKeyRepeatStartDelay;
    }

    gMain.heldKeysRaw = keyInput;
    gMain.heldKeys = gMain.heldKeysRaw;

    // Remap L to A if the L=A option is enabled.
    if (gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
    {
        if (JOY_NEW(L_BUTTON))
            gMain.newKeys |= A_BUTTON;

        if (JOY_HELD(L_BUTTON))
            gMain.heldKeys |= A_BUTTON;
    }

    if (JOY_NEW(gMain.watchedKeysMask))
        gMain.watchedKeysPressed = TRUE;
}

void InitIntrHandlers(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetSerialCallback(NULL);

    REG_IME = 1;

    EnableInterrupts(INTR_FLAG_VBLANK);
}

void SetVBlankCallback(IntrCallback callback)
{
    gMain.vblankCallback = callback;
}

void SetHBlankCallback(IntrCallback callback)
{
    gMain.hblankCallback = callback;
}

void RestoreSerialTimer3IntrHandlers(void)
{
    // gIntrTable[1] = SerialIntr;
    // gIntrTable[2] = Timer3Intr;
}

void SetSerialCallback(IntrCallback callback)
{
    gMain.serialCallback = callback;
}

void FrameUpdate(void)
{
    if (gWirelessCommType != 0)
        RfuVSync();
    else if (gLinkVSyncDisabled == FALSE)
        LinkVSync();

    gMain.vblankCounter1++;

    if (gTrainerHillVBlankCounter && *gTrainerHillVBlankCounter < 0xFFFFFFFF)
        (*gTrainerHillVBlankCounter)++;

    if (gMain.vblankCallback)
        gMain.vblankCallback();

    gMain.vblankCounter2++;

    TryReceiveLinkBattleData();

    if (!gMain.inBattle || !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_FRONTIER | BATTLE_TYPE_RECORDED)))
        Random();

    UpdateWirelessStatusIndicatorSprite();

    INTR_CHECK |= INTR_FLAG_VBLANK;
    gMain.intrCheck |= INTR_FLAG_VBLANK;
}

void InitFlashTimer(void)
{
    // SetFlashTimerIntr(2, gIntrTable + 0x7);
}

void DoHBlankUpdate(void)
{
    if (gMain.hblankCallback)
        gMain.hblankCallback();
}

static void SerialIntr(void)
{
    if (gMain.serialCallback)
        gMain.serialCallback();

    INTR_CHECK |= INTR_FLAG_SERIAL;
    gMain.intrCheck |= INTR_FLAG_SERIAL;
}

void SetTrainerHillVBlankCounter(u32 *counter)
{
    gTrainerHillVBlankCounter = counter;
}

void ClearTrainerHillVBlankCounter(void)
{
    gTrainerHillVBlankCounter = NULL;
}

void DoSoftReset(void)
{
    REG_IME = 0;
    m4aSoundVSyncOff();
    ScanlineEffect_Stop();
    SiiRtcProtect();
    SoftReset(RESET_ALL);
}

void ClearPokemonCrySongs(void)
{
    CpuFill16(0, gPokemonCrySongs, MAX_POKEMON_CRIES * sizeof(struct PokemonCrySong));
}
