#include "gameboy.h"

// placed here to prevent an include loop
#include "bootrom.h"
#include "timer.h"

#ifdef BLARGG
#include <stdio.h>
#endif

// See gameboy.h
int gameboy_create(gameboy_t* gameboy, const char* filename)
{

    M_REQUIRE_NON_NULL(gameboy);
    M_REQUIRE_NON_NULL(filename);

    // Resetting bus
    memset(&(gameboy->bus),0,BUS_SIZE*sizeof(data_t*));

    // Initialising gameboy components and plugging them to the bus
    gameboy->nb_components = 0;

    M_EXIT_IF_ERR(add_gameboy_component(0,gameboy,WORK_RAM_START,WORK_RAM_END));

    M_EXIT_IF_ERR(add_gameboy_component(1,gameboy,REGISTERS_START,REGISTERS_END));

    M_EXIT_IF_ERR(add_gameboy_component(2,gameboy,EXTERN_RAM_START,EXTERN_RAM_END));

    M_EXIT_IF_ERR(add_gameboy_component(3,gameboy,VIDEO_RAM_START,VIDEO_RAM_END));

    M_EXIT_IF_ERR(add_gameboy_component(4,gameboy,GRAPH_RAM_START,GRAPH_RAM_END));

    M_EXIT_IF_ERR(add_gameboy_component(5,gameboy,USELESS_START,USELESS_END));

    component_t echo_ram = {NULL, 0, 0};
    M_EXIT_IF_ERR(component_shared(&echo_ram, &gameboy->components[0])); // share work ram memory with echo ram
    M_EXIT_IF_ERR(bus_plug(gameboy->bus, &echo_ram, ECHO_RAM_START, ECHO_RAM_END)); // plug echo ram into bus

    // Initialising cpu and plugging it to the bus
    M_EXIT_IF_ERR(cpu_init(&(gameboy->cpu)));
    M_EXIT_IF_ERR(cpu_plug(&(gameboy->cpu), &(gameboy->bus)));
    gameboy->cycles = 0; // start cycle count

    // Initialising cartridge and plugging it to the bus
    M_EXIT_IF_ERR(cartridge_init(&(gameboy->cartridge), filename)); // create cartridge
    M_EXIT_IF_ERR(cartridge_plug(&(gameboy->cartridge), gameboy->bus));// plug cartridge to bus

    // Initialising timer
    M_EXIT_IF_ERR(timer_init(&(gameboy->timer), &(gameboy->cpu))); // create timer

    // Initialising joypad
    M_EXIT_IF_ERR(joypad_init_and_plug(&(gameboy->pad),&(gameboy->cpu))); // create and plug joypad

    // Initialising screen and plugging it to the bus
    M_EXIT_IF_ERR(lcdc_init(gameboy)); // create screen
    M_EXIT_IF_ERR(lcdc_plug(&(gameboy->screen),gameboy->bus)); // plug screen to bus

    //Initialising boot
    gameboy->boot=1; // set in bootmode
    M_EXIT_IF_ERR(bootrom_init(&(gameboy->bootrom))); // create boot rom
    M_EXIT_IF_ERR(bootrom_plug(&(gameboy->bootrom), gameboy->bus)); // plug boot rom into bus

    return ERR_NONE;
}

// See gameboy.h
int add_gameboy_component(int component_number, gameboy_t* gameboy, addr_t start, addr_t end)
{
    int errcode = component_create(&gameboy->components[component_number], end - start +1);
    if(errcode!=ERR_NONE) {
        return errcode;
    }
    errcode = bus_plug(gameboy->bus, &gameboy->components[component_number], start, end);
    if(errcode!=ERR_NONE) {
        return errcode;
    }
    gameboy->nb_components+=1;
    return ERR_NONE;
}

// See gameboy.h
void gameboy_free(gameboy_t* gameboy)
{
    for(int i = 0; i < GB_NB_COMPONENTS; ++i) {
        bus_unplug(gameboy->bus, &gameboy->components[i]); // unplug all components from bus
        component_free(&gameboy->components[i]); // free component memory
    }
    if(gameboy->boot==1) { // if ending before the end of the boot
        bootrom_unplug(&(gameboy->bootrom),gameboy->bus);
        bootrom_free(&(gameboy->bootrom));
    }
    cpu_free(&(gameboy->cpu));
    component_free(&(gameboy->bootrom));
    cartridge_free(&(gameboy->cartridge));
    lcdc_free(&(gameboy->screen));
}

#ifdef BLARGG
static int blargg_bus_listener(gameboy_t* gameboy, addr_t addr)
{
    M_REQUIRE_NON_NULL(gameboy);
    if(addr == BLARGG_REG) {
        printf("%c", *(gameboy->bus[addr]));
    }
    return ERR_NONE;
}
#endif

// See gameboy.h
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle)
{
    for(int i=1; i<cycle; ++i) {

        M_EXIT_IF_ERR(timer_cycle(&(gameboy->timer)));
        M_EXIT_IF_ERR(cpu_cycle(&(gameboy->cpu)));
        M_EXIT_IF_ERR(lcdc_cycle(&(gameboy->screen),gameboy->cycles));
        ++(gameboy->cycles);

        M_EXIT_IF_ERR(timer_bus_listener(&(gameboy->timer), gameboy->cpu.write_listener));
        M_EXIT_IF_ERR(bootrom_bus_listener(gameboy, gameboy->cpu.write_listener));
        M_EXIT_IF_ERR(joypad_bus_listener(&(gameboy->pad),gameboy->cpu.write_listener));
        M_EXIT_IF_ERR(lcdc_bus_listener(&(gameboy->screen),gameboy->cpu.write_listener));
#ifdef BLARGG
        M_EXIT_IF_ERR(blargg_bus_listener(gameboy, gameboy->cpu.write_listener));
#endif
    }

    return ERR_NONE;
}
