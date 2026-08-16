#ifndef GUARD_ALLOC_H
#define GUARD_ALLOC_H


#define FREE_AND_SET_NULL(ptr)          \
{                                       \
    Free(ptr);                          \
    ptr = NULL;                         \
}

#define TRY_FREE_AND_SET_NULL(ptr) if (ptr != NULL) FREE_AND_SET_NULL(ptr)

#ifndef PORTABLE
#define HEAP_SIZE 0x1C000

extern u8 gHeap[HEAP_SIZE];
#endif

#define MEMDEBUG

#ifdef MEMDEBUG
void *MemAlloc(u32 size, const char *file, int line);
void *MemAllocZeroed(u32 size, const char *file, int line);

#define Alloc(size) MemAlloc((size), __FILE__, __LINE__)
#define AllocZeroed(size) MemAllocZeroed((size), __FILE__, __LINE__)
#else
void *MemAlloc(u32 size);
void *MemAllocZeroed(u32 size);

#define Alloc(size) MemAlloc((size))
#define AllocZeroed(size) MemAllocZeroed((size))
#endif

void Free(void *pointer);
void InitHeap(void);

#endif // GUARD_ALLOC_H
