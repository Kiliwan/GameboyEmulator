#include "memory.h"

#ifndef SIZE_MAX
#define SIZE_MAX (~(size_t)0)
#endif

// See memory.h
int mem_create(memory_t* mem, size_t size)
{
    M_REQUIRE_NON_NULL(mem);

    if(size==0) {
        return ERR_BAD_PARAMETER;
    }

    if(SIZE_MAX/sizeof(data_t) < size) { // if more than maximum allocatable space
        return ERR_MEM;
    }

    data_t* memory = calloc(size,sizeof(data_t)); // allocate space for memory

    if(memory==NULL) { // if allocation failed
        return ERR_MEM;
    }

    mem->memory = memory;
    mem->size = size;

    return ERR_NONE;
}

// See memory.h
void mem_free(memory_t* mem)
{
    if(mem != NULL) {

        free(mem->memory); // free space of memory

        mem->memory = NULL;

        mem->size=0;
    }
}
