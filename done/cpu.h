#pragma once

/**
 * @file cpu.h
 * @brief CPU model for PPS-GBemul project, high level interface
 *
 * @author J.-C. Chappelier & C. Hölzl, EPFL
 * @date 2019
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> // for error messages

#include "alu.h"
#include "bus.h"
#include "error.h"
#include "opcode.h"

//=========================================================================
/**
 * @brief Type to represent CPU interupts
 */
typedef enum {
    VBLANK, LCD_STAT, TIMER, SERIAL, JOYPAD
} interrupt_t ;


#define REG_IF          0xFF0F
#define REG_IE          0xFFFF
#define HIGH_RAM_START   0xFF80
#define HIGH_RAM_END     0xFFFE
#define HIGH_RAM_SIZE ((HIGH_RAM_END - HIGH_RAM_START)+1)

/**
 * @brief size of the interruption registers
 */
#define IREG_SIZE 8

/**
 * @brief address start to where interruption are treated
 */
#define INTERRUPTION_START 0x40
/**
 * @brief cycles to treat interruption
 */
#define INTERRUPTION_CYCLES 5

//=========================================================================
/**
 * @brief Type to represent CPU
 */
typedef struct cpu_t {
    // Declaration of internal registers

    union {
        struct {
            uint8_t F;
            uint8_t A;
        };
        uint16_t AF;
    };

    union {
        struct {
            uint8_t C;
            uint8_t B;
        };
        uint16_t BC;
    };

    union {
        struct {
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };

    union {
        struct {
            uint8_t L;
            uint8_t H;
        };
        uint16_t HL;
    };

    uint16_t PC; // program counter
    uint16_t SP; // stack pointer

    alu_output_t alu;

    bus_t* bus;

    // interruption registers
    uint8_t IME;
    uint8_t IE;
    uint8_t IF;

    uint8_t HALT;

    component_t high_ram;

    // listener
    addr_t write_listener;

    uint8_t idle_time;

} cpu_t ;

//=========================================================================
/**
 * @brief Run one CPU cycle
 * @param cpu (modified), the CPU which shall run
 * @param cycle, the cycle number to run, starting from 0
 * @return error code
 */
int cpu_cycle(cpu_t* cpu);


/**
 * @brief Plugs a bus into the cpu
 *
 * @param cpu cpu to plug into
 * @param bus bus to plug
 *
 * @return error code
 */
int cpu_plug(cpu_t* cpu, bus_t* bus);


/**
 * @brief Starts the cpu by initializing all registers at zero
 *
 * @param cpu cpu to start
 *
 * @return error code
 */
int cpu_init(cpu_t* cpu);


/**
 * @brief Frees a cpu
 *
 * @param cpu cpu to free
 */
void cpu_free(cpu_t* cpu);


/**
 * @brief Set an interruption
 */
void cpu_request_interrupt(cpu_t* cpu, interrupt_t i);

/**
 * @brief Check if there is a bit equal to 1 in IE and IF
 *
 * @param cpu cpu to check
 *
 * @return index of the bit or -1 if none found
 */
int IF_IE_compare(cpu_t* cpu);

/**
 * @brief Check flag conditions
 *
 * @param cc condition to check, flags flags to check condition on
 *
 * @return 1 if condition tested true, 0 if false
 */
int test_cc(int cc, flags_t flags);

#ifdef __cplusplus
}
#endif
