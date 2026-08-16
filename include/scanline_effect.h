#ifndef GUARD_SCANLINE_EFFECT_H
#define GUARD_SCANLINE_EFFECT_H

#define SCANLINE_EFFECT_BG0HOFS 0
#define SCANLINE_EFFECT_BG0VOFS 1
#define SCANLINE_EFFECT_BG1HOFS 2
#define SCANLINE_EFFECT_BG1VOFS 3
#define SCANLINE_EFFECT_BG2HOFS 4
#define SCANLINE_EFFECT_BG2VOFS 5
#define SCANLINE_EFFECT_BG3HOFS 6
#define SCANLINE_EFFECT_BG3VOFS 7

struct ScanlineEffectParams
{
    u8 effTarget;
    u32 effParam;
    u8 initState;
    u8 unused9;
};

struct ScanlineEffect
{
    void *dmaSrcBuffers[2];
    u8 effTarget;
    u32 effParam;
    u8 srcBuffer;
    u8 state;
    u8 waveTaskId;
};

extern struct ScanlineEffect gScanlineEffect;

extern u32 gScanlineEffectRegBuffers[2][DISPLAY_WIDTH * DISPLAY_HEIGHT];

void ScanlineEffect_Stop(void);
void ScanlineEffect_Clear(void);
void ScanlineEffect_SetParams(struct ScanlineEffectParams params);
void ScanlineEffect_InitHBlankDmaTransfer(void);
u8 ScanlineEffect_InitWave(u16 startLine, u16 endLine, u8 frequency, u8 amplitude, u8 delayInterval, u8 regOffset, bool8 applyBattleBgOffsets);

#endif // GUARD_SCANLINE_EFFECT_H
