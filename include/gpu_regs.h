#ifndef GUARD_GPU_REGS_H
#define GUARD_GPU_REGS_H

// Exported type declarations

// Exported RAM declarations

// Exported ROM declarations
void InitGpuRegManager(void);
void EnableInterrupts(u16 mask);
void DisableInterrupts(u16 mask);

#include "gpu_main.h"

#endif //GUARD_GPU_REGS_H
