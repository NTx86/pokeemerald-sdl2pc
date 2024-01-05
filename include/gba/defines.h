#ifndef GUARD_GBA_DEFINES_H
#define GUARD_GBA_DEFINES_H

#include <stddef.h>

#define TRUE  1
#define FALSE 0

#ifdef PORTABLE
#define IWRAM_DATA
#define EWRAM_DATA
#else
#define IWRAM_DATA __attribute__((section("iwram_data")))
#define EWRAM_DATA __attribute__((section("ewram_data")))
#endif
#define UNUSED __attribute__((unused))

#if MODERN
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

#define ALIGNED(n) __attribute__((aligned(n)))

#define PLTT_SIZE 0x400

#ifndef PORTABLE
#define SOUND_INFO_PTR (*(struct SoundInfo **)0x3007FF0)
#define INTR_CHECK     (*(u16 *)0x3007FF8)
#define INTR_VECTOR    (*(void **)0x3007FFC)

#define EWRAM_START 0x02000000
#define EWRAM_END   (EWRAM_START + 0x40000)
#define IWRAM_START 0x03000000
#define IWRAM_END   (IWRAM_START + 0x8000)

#define PLTT      0x5000000
#else
extern struct SoundInfo * SOUND_INFO_PTR;
extern unsigned short INTR_CHECK;
extern void * INTR_VECTOR;

extern unsigned char PLTT[PLTT_SIZE];
#endif

#define BG_PLTT      PLTT
#define BG_PLTT_SIZE 0x200

#define OBJ_PLTT      (PLTT + 0x200)
#define OBJ_PLTT_SIZE 0x200

#define VRAM_SIZE 0x18000
#ifndef PORTABLE
#define VRAM      0x6000000
#else
extern unsigned char VRAM_[VRAM_SIZE];
#define VRAM (u32)VRAM_
#endif

#define BG_VRAM           VRAM
#define BG_VRAM_SIZE      0x10000
#define BG_CHAR_SIZE      0x4000
#define BG_SCREEN_SIZE    0x800
#define BG_CHAR_ADDR(n)   (BG_VRAM + (BG_CHAR_SIZE * (n)))
#define BG_SCREEN_ADDR(n) (BG_VRAM + (BG_SCREEN_SIZE * (n)))

#define BG_TILE_H_FLIP(n) (0x400 + (n))
#define BG_TILE_V_FLIP(n) (0x800 + (n))

#define NUM_BACKGROUNDS 4

// text-mode BG
#define OBJ_VRAM0      (VRAM + 0x10000)
#define OBJ_VRAM0_SIZE 0x8000

// bitmap-mode BG
#define OBJ_VRAM1      (VRAM + 0x14000)
#define OBJ_VRAM1_SIZE 0x4000

#define OAM_SIZE 0x400
#ifndef PORTABLE
#define OAM      0x7000000
#else
extern unsigned char OAM[OAM_SIZE];
#endif

#define ROM_HEADER_SIZE   0xC0

#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 160

#define DISPLAY_TILE_WIDTH  (DISPLAY_WIDTH / 8)
#define DISPLAY_TILE_HEIGHT (DISPLAY_HEIGHT / 8)

#define TILE_SIZE_4BPP 32
#define TILE_SIZE_8BPP 64

#define TILE_OFFSET_4BPP(n) ((n) * TILE_SIZE_4BPP)
#define TILE_OFFSET_8BPP(n) ((n) * TILE_SIZE_8BPP)

#define TOTAL_OBJ_TILE_COUNT 1024

#endif // GUARD_GBA_DEFINES_H
