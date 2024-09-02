#include "global.h"
#include "battle.h"
#include "data.h"
#include "task.h"
#include "trig.h"
#include "scanline_effect.h"
#include "gpu_main.h"

// EWRAM vars

// Per-scanline register values.
// This is double buffered so that it can be safely written to at any time
// without overwriting the buffer that the DMA is currently reading
EWRAM_DATA u32 gScanlineEffectRegBuffers[2][DISPLAY_WIDTH * DISPLAY_HEIGHT] = {0};

EWRAM_DATA struct ScanlineEffect gScanlineEffect = {0};
EWRAM_DATA static bool8 sShouldStopWaveTask = FALSE;

void ScanlineEffect_Stop(void)
{
    gScanlineEffect.state = 0;
    ClearGpuScanlineEffect();
    if (gScanlineEffect.waveTaskId != TASK_NONE)
    {
        DestroyTask(gScanlineEffect.waveTaskId);
        gScanlineEffect.waveTaskId = TASK_NONE;
    }
}

void ScanlineEffect_Clear(void)
{
    CpuFill16(0, gScanlineEffectRegBuffers, sizeof(gScanlineEffectRegBuffers));
    gScanlineEffect.dmaSrcBuffers[0] = NULL;
    gScanlineEffect.dmaSrcBuffers[1] = NULL;
    gScanlineEffect.effTarget = 0;
    gScanlineEffect.effParam = 0;
    gScanlineEffect.srcBuffer = 0;
    gScanlineEffect.state = 0;
    gScanlineEffect.waveTaskId = TASK_NONE;
}

void ScanlineEffect_SetParams(struct ScanlineEffectParams params)
{
    // Set the DMA src to the value for the second scanline because the
    // first DMA transfer occurs in HBlank *after* the first scanline is drawn
    gScanlineEffect.dmaSrcBuffers[0] = (u32 *)gScanlineEffectRegBuffers[0] + 1;
    gScanlineEffect.dmaSrcBuffers[1] = (u32 *)gScanlineEffectRegBuffers[1] + 1;

    gScanlineEffect.effParam  = params.effParam;
    gScanlineEffect.effTarget = params.effTarget;
    gScanlineEffect.state     = params.initState;
}

void ScanlineEffect_InitHBlankDmaTransfer(void)
{
    if (gScanlineEffect.state == 0)
    {
        return;
    }
    else if (gScanlineEffect.state == 3)
    {
        gScanlineEffect.state = 0;
        ClearGpuScanlineEffect();
        sShouldStopWaveTask = TRUE;
    }
    else
    {
        ClearGpuScanlineEffect();
        // Set DMA to copy to dest register on each HBlank for the next frame.
        // The HBlank DMA transfers do not occurr during VBlank, so the transfer
        // will begin on the HBlank after the first scanline
        SetGpuScanlineEffect(gScanlineEffect.effTarget, gScanlineEffect.effParam, gScanlineEffect.dmaSrcBuffers[gScanlineEffect.srcBuffer]);
        // Manually set the reg for the first scanline
        GpuRefreshScanlineEffect();
        // Swap current buffer
        gScanlineEffect.srcBuffer ^= 1;
    }
}

#define tStartLine            data[0]
#define tEndLine              data[1]
#define tWaveLength           data[2]
#define tSrcBufferOffset      data[3]
#define tFramesUntilMove      data[4]
#define tDelayInterval        data[5]
#define tRegOffset            data[6]
#define tApplyBattleBgOffsets data[7]

static void TaskFunc_UpdateWavePerFrame(u8 taskId)
{
    int value = 0;
    int i;
    int offset;

    if (sShouldStopWaveTask)
    {
        DestroyTask(taskId);
        gScanlineEffect.waveTaskId = TASK_NONE;
    }
    else
    {
        if (gTasks[taskId].tApplyBattleBgOffsets)
        {
            switch (gTasks[taskId].tRegOffset)
            {
            case SCANLINE_EFFECT_BG0HOFS:
                value = gBattle_BG0_X;
                break;
            case SCANLINE_EFFECT_BG0VOFS:
                value = gBattle_BG0_Y;
                break;
            case SCANLINE_EFFECT_BG1HOFS:
                value = gBattle_BG1_X;
                break;
            case SCANLINE_EFFECT_BG1VOFS:
                value = gBattle_BG1_Y;
                break;
            case SCANLINE_EFFECT_BG2HOFS:
                value = gBattle_BG2_X;
                break;
            case SCANLINE_EFFECT_BG2VOFS:
                value = gBattle_BG2_Y;
                break;
            case SCANLINE_EFFECT_BG3HOFS:
                value = gBattle_BG3_X;
                break;
            case SCANLINE_EFFECT_BG3VOFS:
                value = gBattle_BG3_Y;
                break;
            }
        }
        if (gTasks[taskId].tFramesUntilMove != 0)
        {
            gTasks[taskId].tFramesUntilMove--;
            offset = gTasks[taskId].tSrcBufferOffset + DISPLAY_HEIGHT;
            for (i = gTasks[taskId].tStartLine; i < gTasks[taskId].tEndLine; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = gScanlineEffectRegBuffers[0][offset] + value;
                offset++;
            }
        }
        else
        {
            gTasks[taskId].tFramesUntilMove = gTasks[taskId].tDelayInterval;
            offset = gTasks[taskId].tSrcBufferOffset + DISPLAY_HEIGHT;
            for (i = gTasks[taskId].tStartLine; i < gTasks[taskId].tEndLine; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = gScanlineEffectRegBuffers[0][offset] + value;
                offset++;
            }

            // increment src buffer offset
            gTasks[taskId].tSrcBufferOffset++;
            if (gTasks[taskId].tSrcBufferOffset == gTasks[taskId].tWaveLength)
                gTasks[taskId].tSrcBufferOffset = 0;
        }
    }
}

static void GenerateWave(u32 *buffer, u8 frequency, u8 amplitude, u16 unused)
{
    u16 i = 0;
    u8 theta = 0;

    while (i < DISPLAY_HEIGHT)
    {
        buffer[i] = (gSineTable[theta % 320] * amplitude) / 256;
        theta += frequency;
        i++;
    }
}

// Initializes a background "wave" effect that affects scanlines startLine (inclusive) to endLine (exclusive).
// 'frequency' and 'amplitude' control the frequency and amplitude of the wave.
// 'delayInterval' controls how fast the wave travels up the screen. The wave will shift upwards one scanline every 'delayInterval'+1 frames.
// 'regOffset' is the offset of the video register to modify.
u8 ScanlineEffect_InitWave(u16 startLine, u16 endLine, u8 frequency, u8 amplitude, u8 delayInterval, u8 regOffset, bool8 applyBattleBgOffsets)
{
    int i;
    int offset;
    struct ScanlineEffectParams params;
    u8 taskId;

    ScanlineEffect_Clear();

    if (regOffset & 1)
        params.effTarget = GPU_SCANLINE_EFFECT_BGY;
    else
        params.effTarget = GPU_SCANLINE_EFFECT_BGX;
    params.effParam = regOffset / 2;

    params.initState = 1;
    params.unused9 = 0;
    ScanlineEffect_SetParams(params);

    taskId = CreateTask(TaskFunc_UpdateWavePerFrame, 0);

    gTasks[taskId].tStartLine            = startLine;
    gTasks[taskId].tEndLine              = endLine;
    gTasks[taskId].tWaveLength           = 256 / frequency;
    gTasks[taskId].tSrcBufferOffset      = 0;
    gTasks[taskId].tFramesUntilMove      = delayInterval;
    gTasks[taskId].tDelayInterval        = delayInterval;
    gTasks[taskId].tRegOffset            = regOffset;
    gTasks[taskId].tApplyBattleBgOffsets = applyBattleBgOffsets;

    gScanlineEffect.waveTaskId = taskId;
    sShouldStopWaveTask = FALSE;

    GenerateWave(&gScanlineEffectRegBuffers[0][DISPLAY_HEIGHT], frequency, amplitude, endLine - startLine);

    offset = DISPLAY_HEIGHT;
    for (i = startLine; i < endLine; i++)
    {
        gScanlineEffectRegBuffers[0][i] = gScanlineEffectRegBuffers[0][offset];
        gScanlineEffectRegBuffers[1][i] = gScanlineEffectRegBuffers[0][offset];
        offset++;
    }

    return taskId;
}
