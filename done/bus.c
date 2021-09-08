#include "bus.h"

// See bus.h
int bus_remap(bus_t bus, component_t* c, addr_t offset)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem->memory);

    if((c->start <= c->end) && ((c->end - c->start + offset)<(c->mem->size))) {
        for(int i=0; i<=(c->end-c->start); ++i) {
            bus[c->start+i]=&(c->mem->memory[offset+i]); // map all memory addresses to the bus
        }
        return ERR_NONE;
    } else {
        return ERR_ADDRESS;
    }
}

// See bus.h
int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);

    if((start <= end) && ((end-start+offset)<(c->mem->size))) {
        c->start=start;
        c->end=end;
        int result = bus_remap(bus,c,offset);
        if(result==ERR_ADDRESS) { // if mapping failed
            c->start=0;
            c->end=0;
            return ERR_ADDRESS;
        }
        return ERR_NONE;
    } else {
        return ERR_ADDRESS;
    }
}

// See bus.h
int bus_plug(bus_t bus, component_t* c, addr_t start, addr_t end)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);

    for(int i=start; i<=end; i++) {
        if(bus[i]!=NULL) { // test if all addresses are empty
            return ERR_ADDRESS;
        }
    }
    return bus_forced_plug(bus, c, start, end, 0);
}

// See bus.h
int bus_unplug(bus_t bus, component_t* c)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);

    for(int i=c->start; i<=c->end; i++) {
        bus[i]=NULL; // reset all addresses
    }
    c->start=0;
    c->end=0;
    return ERR_NONE;
}

// See bus.h
int bus_read(const bus_t bus, addr_t address, data_t* data)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(data);

    if(bus[address]==NULL) {
        *data=0xFF; // error code if address invalid according to the gameboy implementation
    } else {
        *data=*bus[address];
    }
    return ERR_NONE;
}

// See bus.h
int bus_write(bus_t bus, addr_t address, data_t data)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(bus[address]);

#ifdef TETRIS_ROM_WRITE_CHECK
    if(address<0x8000) return ERR_NONE; // only necessary for the tetris game
#endif

    *bus[address]=data;

    return ERR_NONE;
}

// See bus.h
int bus_read16(const bus_t bus, addr_t address, addr_t* data16)
{
    M_REQUIRE_NON_NULL(data16);

    data_t first = 0;
    data_t second = 0;
    int errcode1 = bus_read(bus,address,&first);
    int errcode2 = bus_read(bus,address+1,&second);

    // test if errors in any of the the parts
    if(errcode1!=ERR_NONE) {
        return errcode1;
    }
    if(errcode2!=ERR_NONE) {
        return errcode2;
    }

    *data16 = merge8(first,second);
    return ERR_NONE;
}

// See bus.h
int bus_write16(bus_t bus, addr_t address, addr_t data16)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(bus[address]);

#ifdef TETRIS_ROM_WRITE_CHECK
    if(address<0x8000) return ERR_NONE; // only necessary for the tetris game
#endif

    data_t first = msb8(data16);
    data_t second = lsb8(data16);
    int errcode1 = bus_write(bus,address,second);
    int errcode2 = bus_write(bus,address+1,first);

    // test if errors in any of the the parts
    if(errcode1!=ERR_NONE) {
        return errcode1;
    }
    if(errcode2!=ERR_NONE) {
        return errcode2;
    }

    return ERR_NONE;
}

