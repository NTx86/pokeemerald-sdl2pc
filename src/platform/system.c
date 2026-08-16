#ifdef PORTABLE
#include "global.h"
#include "platform/dma.h"
#include "m4a.h"

u16 INTR_CHECK;
void *INTR_VECTOR;
unsigned char REG_BASE[0x400] __attribute__ ((aligned (4)));
unsigned char FLASH_BASE[131072*4] __attribute__ ((aligned (4))); //base gba flash_base multiplied by 8 for extra storage
struct SoundInfo *SOUND_INFO_PTR;

extern void (*const gIntrTable[])(void);

void AudioUpdate(void)
{
	if (gSoundInit == FALSE)
		return;

	m4aSoundMain();
	m4aSoundVSync();
}
#endif