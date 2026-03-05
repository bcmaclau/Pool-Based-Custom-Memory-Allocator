#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void* my_malloc(size_t size);
void my_free(void* ptr);

#endif