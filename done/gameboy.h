#pragma once

/**
 * @file gameboy.h
 * @brief Gameboy Header for GameBoy Emulator
 *
 * @author C. Hölzl, EPFL
 * @date 2019
 */
#include <string.h> // for memset

#include "bus.h"
#include "component.h"
#include "cartridge.h"
#include "timer.h"
#include "error.h"
#include "lcdc.h"
#include "joypad.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief total number of gameboy components in the components array
 */
#define GB_NB_COMPONENTS 6

/**
 * @brief Game Boy data structure.
 *        Regroups everything needed to simulate the Game Boy.
 */
typedef struct gameboy_ {
    bus_t bus;
    cpu_t cpu;
    uint64_t cycles;
    gbtimer_t timer;
    cartridge_t cartridge;
    component_t components[GB_NB_COMPONENTS];
    uint8_t nb_components;
    component_t bootrom;
    uint8_t boot;
    lcdc_t screen;
    joypad_t pad;
} gameboy_;


// Number of Game Boy cycles per second (= 2^20)
#define GB_CYCLES_PER_S  (((uint64_t) 1) << 20)

/**
 * @brief Creates a gameboy
 *
 * @param gameboy pointer to gameboy to create
 */
int gameboy_create(gameboy_t* gameboy, const char* filename);

/**
 * @brief Add a new component to the gameboy components array and plugs it to the bus
 *
 * @param number of the component to add in the array
 * @param gameboy pointer to add components to
 * @param start of the component in the bus
 * @param end of the component in the bus
 *
 * @return error code
 */
int add_gameboy_component(int component_number, gameboy_t* gameboy, addr_t start, addr_t end);

/**
 * @brief Destroys a gameboy
 *
 * @param gameboy pointer to gameboy to destroy
 * @return error code
 */
void gameboy_free(gameboy_t* gameboy);

/**
 * @brief Runs a gamefor for/until a given cycle
 *
 * @param gameboy pointer to run
 * @param cycles to run
 * @return error code
 */
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle);

/**
 * @brief Adresses of the GameBoy
 *
 */
#define MEM_SIZE(X) (X ## _END - X ## _START + 1)

#define BOOT_ROM_START   0x0000
#define BOOT_ROM_END     0x00FF

#define VIDEO_RAM_START  0x8000
#define VIDEO_RAM_END    0x9FFF

#define EXTERN_RAM_START 0xA000
#define EXTERN_RAM_END   0xBFFF

#define WORK_RAM_START   0xC000
#define WORK_RAM_END     0xDFFF

#define ECHO_RAM_START   0xE000
#define ECHO_RAM_END     0xFDFF

#define GRAPH_RAM_START  0xFE00
#define GRAPH_RAM_END    0xFE9F

#define USELESS_START    0xFEA0
#define USELESS_END      0xFEFF

#define REGISTERS_START  0xFF00
#define REGISTERS_END    0xFF7F


// Memory-mapped "IO" registers
#define REGS_START      0xFF00
#define BLARGG_REG      0xFF01

#define REGS_LCDC_START 0xFF40
#define REGS_LCDC_END   0xFF4C
#define REG_BOOT_ROM_DISABLE  0xFF50


#ifdef __cplusplus
}
#endif
