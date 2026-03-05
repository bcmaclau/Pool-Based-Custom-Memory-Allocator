#include "../include/allocator.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

//Tests storing and freeing values in every pool
void test_basic_alloc_and_dealloc() {
    size_t headerSize = sizeof(size_t);

    for (int i = 0; i < 9; i++) {
        int poolSize = ((1 << (i + 4)) - headerSize);
        char* allocation = (char*)my_malloc(poolSize);
        for (int j = 0; j < poolSize; j++) {
            allocation[j] = (char)i;
        }
        for (int j = 0; j < poolSize; j++) {
            assert(allocation[j] == (char)i);
        }
        my_free((void*)allocation);
    }

    printf("test_basic_alloc_and_dealloc passed\n");
}

//Tests allocating more than 1 page of memory from a pool
void test_exhausting_pool() {
    size_t headerSize = sizeof(size_t);
    int poolSize = (1 << 11) - headerSize;
    
    char* a = (char*)my_malloc(poolSize);
    char* b = (char*)my_malloc(poolSize);
    char* c = (char*)my_malloc(poolSize);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    assert(a != b);
    assert(a != c);
    assert(b != c);

    for (int i = 0; i < poolSize; i++) {
        a[i] = 'a';
    }
    for (int i = 0; i < poolSize; i++) {
        b[i] = 'b';
    }
    for (int i = 0; i < poolSize; i++) {
        c[i] = 'c';
    }
    
    for (int i = 0; i < poolSize; i++) {
        assert(a[i] == 'a');
    }
    for (int i = 0; i < poolSize; i++) {
        assert(b[i] == 'b');
    }
    for (int i = 0; i < poolSize; i++) {
        assert(c[i] == 'c');
    }

    my_free((void*)a);
    my_free((void*)b);
    my_free((void*)c);

    printf("test_exhausting_pool passed\n");
}

//Tests if freed memory gets reallocated
void test_memory_reuse() {
    void* a = my_malloc(24);
    assert(a != NULL);

    my_free(a);

    void* b = my_malloc(24);
    assert(a == b);

    printf("test_memory_reuse passed\n");
}

//Tests allocating sizes larger than 4088 bytes
void test_large_alloc() {
    void* largeAlloc = my_malloc(5000);
    assert(largeAlloc != NULL);

    my_free(largeAlloc);

    printf("test_large_alloc passed\n");
}

//Tests freeing the value NULL
void test_null_free() {
    my_free(NULL);

    printf("test_null_alloc passed\n");
}

//Tests allocating and deallocating from the different pools in different orders
void test_mixed_allocations() {
    void* a = my_malloc(24);
    void* b = my_malloc(120);
    void* c = my_malloc(56);
    void* d = my_malloc(248);

    assert(a != NULL);
    assert(b != NULL);
    assert(c != NULL);
    assert(d != NULL);

    my_free(b);
    my_free(c);

    void* e = my_malloc(120);
    void* f = my_malloc(4080);

    assert(e != NULL);
    assert(f != NULL);

    my_free(a);
    my_free(d);
    my_free(e);
    my_free(f);

    printf("test_mixed_allocations passed\n");
}

//Stress tests the allocator using random values
void test_random_stress() {
    int n = 10000;

    void* ptrs[n];

    for (int i = 0; i < n; i++) {
        size_t randomSize = rand() % 5000;
        ptrs[i] = my_malloc(randomSize);
        assert(ptrs[i] != NULL);
    }

    for (int i = 0; i < n; i++) {
        if (i % 2 == 0) {
            my_free(ptrs[i]);
        }
    }

    for (int i = 0; i < n; i++) {
        if (i % 2 == 1) {
            my_free(ptrs[i]);
        }
    }

    printf("test_random_stress passed\n");
}

int main() {
    test_basic_alloc_and_dealloc();
    test_exhausting_pool();
    test_memory_reuse();
    test_large_alloc();
    test_null_free();
    test_mixed_allocations();
    test_random_stress();
}