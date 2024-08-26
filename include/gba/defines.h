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

#define BG_PLTT_SIZE  0x200
#define OBJ_PLTT      (PLTT + BG_PLTT_SIZE)
#define OBJ_PLTT_SIZE 0x200
#define PLTT_SIZE     (BG_PLTT_SIZE + OBJ_PLTT_SIZE)

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

#define PLTT      ((unsigned char*)(gpu.palette))
#endif

#define BG_PLTT       PLTT

#ifndef PORTABLE
#define VRAM_SIZE 0x18000
#define VRAM      0x6000000
#endif

#define BG_CHAR_SIZE      0x4000
#define BG_SCREEN_SIZE    (0x80000*2)

#define GBA_BG_CHAR_SIZE 0x4000 // Size of a character map on the GBA. Don't touch!
#define GBA_BG_SCREEN_SIZE 0x800 // Size of a BG map on the GBA. Don't touch!

#define BG_CHAR_ADDR(n)   (gpu.gfxData + (BG_CHAR_SIZE * (n)))
#define BG_SCREEN_ADDR(n) (gpu.tileMaps + (BG_SCREEN_SIZE * (n)))

#define BG_TILE_H_FLIP(n) (0x400 + (n))
#define BG_TILE_V_FLIP(n) (0x800 + (n))

#define NUM_BACKGROUNDS 4

// text-mode BG
#define OBJ_VRAM0      (gpu.spriteGfxData)
#define OBJ_VRAM0_SIZE (gpu.spriteGfxDataSize)

// bitmap-mode BG
#define OBJ_VRAM1      (gpu.spriteGfxData)
#define OBJ_VRAM1_SIZE (gpu.spriteGfxDataSize)

#define OAM_SIZE 0x400
#ifndef PORTABLE
#define OAM      0x7000000
#else
#define OAM      (void*)(gpu.spriteList)
#endif

#define ROM_HEADER_SIZE   0xC0

// Dimensions of a tile in pixels
#define TILE_WIDTH  8
#define TILE_HEIGHT 8

// Dimensions of the GBA screen in pixels
#define BASE_DISPLAY_WIDTH  240
#define BASE_DISPLAY_HEIGHT 160

// Max display width
#define DISPLAY_WIDTH  (BASE_DISPLAY_WIDTH * 2)
#define DISPLAY_HEIGHT (BASE_DISPLAY_HEIGHT * 2)

// Dimensions of the GBA screen in tiles
#define DISPLAY_TILE_WIDTH  (BASE_DISPLAY_WIDTH / TILE_WIDTH)
#define DISPLAY_TILE_HEIGHT (BASE_DISPLAY_HEIGHT / TILE_HEIGHT)

// Size of different tile formats in bytes
#define TILE_SIZE(bpp)((bpp) * TILE_WIDTH * TILE_HEIGHT / 8)
#define TILE_SIZE_1BPP TILE_SIZE(1) // 8
#define TILE_SIZE_4BPP TILE_SIZE(4) // 32
#define TILE_SIZE_8BPP TILE_SIZE(8) // 64

#define TILE_OFFSET_4BPP(n) ((n) * TILE_SIZE_4BPP)
#define TILE_OFFSET_8BPP(n) ((n) * TILE_SIZE_8BPP)

#define TOTAL_OBJ_TILE_COUNT 1024

#define PLTT_SIZEOF(n) ((n) * sizeof(u16))
#define PLTT_SIZE_4BPP PLTT_SIZEOF(16)
#define PLTT_SIZE_8BPP PLTT_SIZEOF(256)

#define PLTT_OFFSET_4BPP(n) ((n) * PLTT_SIZE_4BPP)

#endif // GUARD_GBA_DEFINES_H
