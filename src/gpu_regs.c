#include "global.h"
#include "gpu_regs.h"
#include "gpu_main.h"

static vbool8 sGpuRegBufferLocked;
static vbool8 sShouldSyncRegIE;
static vu16 sRegIE;

static void SyncRegIE(void);
static void UpdateRegDispstatIntrBits(u16 regIE);

void InitGpuRegManager(void)
{
    sShouldSyncRegIE = FALSE;
    sRegIE = 0;
}

static void SyncRegIE(void)
{
    if (sShouldSyncRegIE)
    {
        u16 temp = REG_IME;
        REG_IME = 0;
        REG_IE = sRegIE;
        REG_IME = temp;
        sShouldSyncRegIE = FALSE;
    }
}

void EnableInterrupts(u16 mask)
{
    sRegIE |= mask;
    sShouldSyncRegIE = TRUE;
    SyncRegIE();
    UpdateRegDispstatIntrBits(sRegIE);
}

void DisableInterrupts(u16 mask)
{
    sRegIE &= ~mask;
    sShouldSyncRegIE = TRUE;
    SyncRegIE();
    UpdateRegDispstatIntrBits(sRegIE);
}

static void UpdateRegDispstatIntrBits(u16 regIE)
{
    u16 oldValue = GetGpuState(GPU_STATE_DISPSTAT) & (DISPSTAT_HBLANK_INTR | DISPSTAT_VBLANK_INTR);
    u16 newValue = 0;

    if (regIE & INTR_FLAG_VBLANK)
        newValue |= DISPSTAT_VBLANK_INTR;

    if (regIE & INTR_FLAG_HBLANK)
        newValue |= DISPSTAT_HBLANK_INTR;

    if (oldValue != newValue)
        SetGpuState(GPU_STATE_DISPSTAT, newValue);
}
