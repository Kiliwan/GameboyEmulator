#include "component.h"

// See component.h
int component_create(component_t* c, size_t mem_size)
{
    M_REQUIRE_NON_NULL(c);

    if(mem_size==0) { // if empty memory
        c->start=0;
        c->end=0;
        c->mem=NULL;
        return ERR_NONE;
    } else {
        c->start=0;
        c->end=0;

        memory_t* memory = calloc(1,sizeof(memory_t)); // allocate space for memory
        if(memory==NULL) { // test if allocation successful
            return ERR_MEM;
        }
        c->mem=memory;
        return mem_create(c->mem,mem_size); // create memory component
    }
}

// See component.h
void component_free(component_t* c)
{
    if(c != NULL && c->mem != NULL) {
        c->start=0;
        c->end=0;
        mem_free(c->mem); // free memory component
        free(c->mem); // free space of memory
        c->mem=NULL;
    }
}

// See component.h
int component_shared(component_t* c, component_t* c_old)
{
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c_old);

    c->start=0;
    c->end=0;
    c->mem=c_old->mem; // share memory

    M_REQUIRE_NON_NULL(c->mem);

    return ERR_NONE;
}
