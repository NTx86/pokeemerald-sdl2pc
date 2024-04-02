#ifndef GUARD_GPU_REGS_H
#define GUARD_GPU_REGS_H

// Exported type declarations

// Exported RAM declarations

// Exported ROM declarations
void InitGpuRegManager(void);
void CopyBufferedValuesToGpuRegs(void);
void SetGpuReg(u16 regOffset, u16 value);
void SetGpuReg_ForcedBlank(u16 regOffset, u16 value);
u16 GetGpuReg(u16 regOffset);
void SetGpuRegBits(u16 regOffset, u16 mask);
void ClearGpuRegBits(u16 regOffset, u16 mask);
void EnableInterrupts(u16 mask);
void DisableInterrupts(u16 mask);

#endif //GUARD_GPU_REGS_H
