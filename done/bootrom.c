#include "bootrom.h"

//See bootrom.h
int bootrom_init(component_t* c)
{

    M_REQUIRE_NON_NULL(c);

    int errcode = component_create(c, MEM_SIZE(BOOT_ROM));
    if(errcode != ERR_NONE) {
        return errcode;
    }

    data_t bootrom[MEM_SIZE(BOOT_ROM)] = GAMEBOY_BOOT_ROM_CONTENT;

    for(int i = 0; i < c->mem->size; ++i) {
        *(c->mem->memory + i)=bootrom[i]; // assign bootrom content to allocated bootrom component memory
    }

    return ERR_NONE;
}

//See bootrom.h
int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr)
{

    M_REQUIRE_NON_NULL(gameboy);

    if(gameboy->boot && addr==REG_BOOT_ROM_DISABLE) { // if in boot and at the end of the bootrom
        bus_unplug(gameboy->bus, &(gameboy->bootrom));
        bootrom_free(&(gameboy->bootrom));
        cartridge_plug(&(gameboy->cartridge), gameboy->bus);
        gameboy->boot=0;
    }

    return ERR_NONE;
}
