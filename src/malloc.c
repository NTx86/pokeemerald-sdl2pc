#include "global.h"
#include "malloc.h"

#include <stdlib.h>

#ifndef PORTABLE
ALIGNED(4) EWRAM_DATA u8 gHeap[HEAP_SIZE] = {0};
#endif

#define MEMDEBUG

#define MALLOC_SYSTEM_ID 0xA3A3

struct MemBlock {
    // Magic number used for error checking. Should equal MALLOC_SYSTEM_ID.
    u16 magic;

    // Size of the block, including this header struct.
    size_t size;

    // Size of the block (not including this header struct).
    size_t block_size;

    // Previous block pointer.
    struct MemBlock *prev;

    // Next block pointer.
    struct MemBlock *next;

#ifdef MEMDEBUG
    const char *debug_file;
    int debug_line;
#endif
};

static struct MemBlock sHeapHead;
static size_t sHeapUsed;

#ifdef MEMDEBUG
static void *AllocInternal(u32 size, const char *file, int line)
#else
static void *AllocInternal(u32 size)
#endif
{
    struct MemBlock *block = malloc(sizeof(struct MemBlock) + size);
    if (!block)
        return NULL;

    block->block_size = size;
    block->size = sizeof(struct MemBlock) + block->block_size;

#ifdef MEMDEBUG
    block->debug_file = file;
    block->debug_line = line;
#endif

    block->next = sHeapHead.next;
    block->prev = &sHeapHead;
    block->next->prev = block;

    sHeapHead.next = block;
    sHeapUsed += block->block_size;

#ifdef MEMDEBUG
    printf("AllocInternal: Allocated %p, size %d bytes (%s:%d)\n", block, size, file, line);
#endif

    return (void *)((uintptr_t)block + sizeof(struct MemBlock));
}

static void FreeInternal(void *pointer)
{
    if (!pointer)
        return;

    struct MemBlock *block = (struct MemBlock *)((uintptr_t)pointer - sizeof(struct MemBlock));

    block->prev->next = block->next;
    block->next->prev = block->prev;

#ifdef MEMDEBUG
    printf("FreeInternal: Freeing %p, size %d bytes (%s:%d)\n", pointer, block->block_size, block->debug_file, block->debug_line);
#endif

    sHeapUsed -= block->block_size;

    free(block);
}

#ifdef MEMDEBUG
static void *AllocZeroedInternal(u32 size, const char *file, int line)
#else
static void *AllocZeroedInternal(u32 size)
#endif
{
#ifdef MEMDEBUG
    void *mem = AllocInternal(size, file, line);
#else
    void *mem = AllocInternal(size);
#endif

    if (mem == NULL)
    {
#ifdef MEMDEBUG
        printf("AllocZeroedInternal: Failed to allocate %d bytes\n", size);
#endif
        return NULL;
    }

    memset(mem, 0x00, size);

    return mem;
}

bool32 CheckMemBlockInternal(void *pointer)
{
    struct MemBlock *block = (struct MemBlock *)((uintptr_t)pointer - sizeof(struct MemBlock));

    if (block->magic != MALLOC_SYSTEM_ID
    || block->next->prev != block
    || block->prev->next != block)
        return FALSE;

    return TRUE;
}

void InitHeap(void)
{
    if (sHeapUsed)
    {
        struct MemBlock *block, *next;
        void *data;

#ifdef MEMDEBUG
        printf("InitHeap called while %u bytes of memory are in use\n", sHeapUsed);
#endif

        for (block = sHeapHead.next; block != &sHeapHead; block = next)
        {
            next = block->next;
            Free((void *)((uintptr_t)block + sizeof(struct MemBlock)));
        }
    }

    memset(&sHeapHead, 0, sizeof sHeapHead);

    sHeapHead.next = &sHeapHead;
    sHeapHead.prev = sHeapHead.next;

    sHeapUsed = 0;
}

#ifdef MEMDEBUG
void *MemAlloc(u32 size, const char *file, int line)
{
    return AllocInternal(size, file, line);
}

void *MemAllocZeroed(u32 size, const char *file, int line)
{
    return AllocZeroedInternal(size, file, line);
}
#else
void *MemAlloc(u32 size)
{
    return AllocInternal(size);
}

void *MemAllocZeroed(u32 size)
{
    return AllocZeroedInternal(size);
}
#endif

void Free(void *pointer)
{
    FreeInternal(pointer);
}

bool32 CheckMemBlock(void *pointer)
{
    return CheckMemBlockInternal(pointer);
}

bool32 CheckHeap()
{
    struct MemBlock *block = sHeapHead.next;

    for (; block != &sHeapHead; block = block->next)
    {
        void *data = (void *)((uintptr_t)block + sizeof(struct MemBlock));
        if (!CheckMemBlockInternal(data))
            return FALSE;
    }

    return TRUE;
}
