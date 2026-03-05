#Pool-Based Custom Memory Allocator

This project implements a custom fixed-size memory allocator in C using a pool-based design. The allocator manages into power of 2 size blocks ranging from 16-4096 bytes, and provides O(1) allocation and deallocation through singly-linked free lists. Each block produced by the allocator includes an 8 byte header as internal fragmentation for faster deallocation and preserved alignment. Ant allocation larger than 4088 bytes is handled by malloc.

Design:

The allocator contains 9 memory pools of fixed power of 2 sizes.
- Pool 0 -> 16 byte chunks
- Pool 1 -> 32 byte chunks
- Pool 2 -> 64 byte chunks
...
- Pool 9 -> 4096 byte chunks

Each chunk of memory is structed as:
[ Header | Free-list Pointer (when unused) | User Memory ]

The header contains the pool index that the memory was taken from when allocated.
The free-list pointer stores the location of the next available free chunk of memory.
The rest of the chunk is left for the user.

Allocation:

Which pool the memory is taken from is determined by finding the smallest n such that: size + header <= 2^n. If the allocator tries taking memory from a pool that does not exist, a 4096 byte page is allocated to the pool using sbrk, and the free-list for that pool is then created. The memory is taken from the first node in the free-list. Any allocations larger than 4088 bytes (4096 - header) are handled by malloc and marked as a large allocation.

Deallocation:

The pool index is read from the chunk header. If the chunk is marked as a large allocation, the memory is freed using free. Otherwise the memory is stored into the front of the free-list for its given pool.

Compiling Tests: make

Running Tests: make run

Tradeoffs:

Pros
- Allocation and deallocation run in O(1)
- Minimal Fragmentation
- Behavior is predictable

Cons
- Memory is wasted if the user does not allocate at exact chunk sizes
- No protection against freeing the same memory more than once
- Not thread-safe
