#include "../include/allocator.h"

#include <stdio.h>

//Definitions for the page size on x86_64 architecture, the size of each chunk's header, and the header for large allocations.
#define PAGE_SIZE (1 << 12)
#define HEADER_SIZE sizeof(size_t)
#define LARGE_ALLOC __SIZE_MAX__

//8 byte struct to act as the header.
struct Header {
    size_t poolIndex;
};

//There are 9 memory pools with node sizes ranging from 16 bytes, to 4096 bytes.
//The allocator will use malloc if a user requests an amount of memory over 4088 bytes (4096 accounting for header).
//The sizes of nodes increase by 2^(n) for each pool.
struct Header* freeList[9];
int freeListInit = 0;

//Helper function.
//Given the size requested by the user, returns the memory pool to allocate from.
//Returns -1 if the requested size is over 1 page.
int get_pool_index(size_t size) {
    for (int i = 4; i <= 12; i++) {
        if ((int)(size + HEADER_SIZE) <= (1 << i)) {
            return i - 4;
        }
    }
    return -1;
}

//Helper function.
//If a pool is out of memory or hasn't been initialized, it adds 1 page of memory to the pool and creates a free list for that pool.
void init_memory_pool(int poolIndex) {
    size_t nodeSize = 1 << (poolIndex + 4);
    int nodesPerPage = (PAGE_SIZE / nodeSize);

    void* poolStart = sbrk(PAGE_SIZE);

    if (poolStart == (void*)-1) {
        return;
    }

    for (int i = 0; i < nodesPerPage; i++) {
        struct Header* node = (struct Header*)((char*)poolStart + (i * nodeSize));
        node->poolIndex = poolIndex;
        *(struct Header**)((char*)node + HEADER_SIZE) = freeList[poolIndex];
        freeList[poolIndex] = node;
    }
}

//my_malloc follows the same behavior as described in the man pages for malloc.
//First it finds what pool of memory will be returned.
//If the index returns -1 then the user requested more than one page of memory and will be allocated with malloc.
//Else the function will check to see if the right pool has enough memory, if not then it get initialized.
//Then function returns the first MemoryNode in that pool's free list.
void* my_malloc(size_t size) {
    if (!freeListInit) {
        memset(freeList, 0, sizeof(freeList));
        freeListInit = 1;
    }

    int poolIndex = get_pool_index(size);

    if (poolIndex == -1) {
        void* ptr = malloc(size + HEADER_SIZE);
        if (!ptr) {
            return NULL;   
        }
        *(size_t*)ptr = LARGE_ALLOC;
        return (char*)ptr + HEADER_SIZE;
    }

    if (freeList[poolIndex] == NULL) {
        init_memory_pool(poolIndex);
        if (freeList[poolIndex] == 0) {
            return NULL;
        }
    }

    void* freeMem = (void*)((char*)freeList[poolIndex] + HEADER_SIZE);
    freeList[poolIndex] = *(struct Header**)((char*)freeList[poolIndex] + HEADER_SIZE);

    return freeMem;
}

//my_free follows the same behavior describes in the man pages for free.
//First it reads the header to find which pool memory came from.
//If the index reads LARGE_ALLOC, the memory was allocated using malloc and will be freed using free.
//Else the function finds the pool and adds the memory back to the free list for that pool.
void my_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    size_t poolIndex = *(size_t*)((char*)ptr - HEADER_SIZE);

    if (poolIndex == LARGE_ALLOC) {
        free(ptr - HEADER_SIZE);
        return;
    }

    struct Header* newHeader = (struct Header*)((char*)ptr - HEADER_SIZE);
    *(struct Header**)((char*)newHeader + HEADER_SIZE) = freeList[poolIndex];
    freeList[poolIndex] = newHeader;
}