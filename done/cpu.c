/**
 * @file cpu.c
 * @brief Game Boy CPU simulation
 *
 * @date 2019
 */
#include "cpu.h"

// placed here to prevent an include loop
#include "cpu-alu.h"
#include "cpu-registers.h"
#include "cpu-storage.h"

// ======================================================================
// See cpu.h
int cpu_init(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    int err = component_create(&(cpu->high_ram), HIGH_RAM_SIZE);

    if(err != ERR_NONE) {
        return err;
    }

    // Initialize internal registers and ALU
    cpu_AF_set(cpu, 0);
    cpu_BC_set(cpu, 0);
    cpu_DE_set(cpu, 0);
    cpu_HL_set(cpu, 0);

    cpu->SP = 0;
    cpu->PC = 0;

    cpu->alu.flags = 0;
    cpu->alu.value = 0;

    cpu->idle_time=0;

    cpu->IME=0;
    cpu->IF=0;
    cpu->IE=0;

    cpu->HALT=0;

    cpu->write_listener=0;

    return ERR_NONE;
}

// ======================================================================
// See cpu.h
int cpu_plug(cpu_t* cpu, bus_t* bus)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(bus);

    cpu->bus=bus; // plug cpu to bus

    // Allow access to interruption registers from bus
    (*(bus))[REG_IF] = &(cpu->IF);
    (*(bus))[REG_IE] = &(cpu->IE);

    return bus_plug(*(cpu->bus), &(cpu->high_ram), HIGH_RAM_START, HIGH_RAM_END);
}

// ======================================================================
// See cpu.h
void cpu_free(cpu_t* cpu)
{
    if(cpu != NULL) {
        bus_unplug(*(cpu->bus), &(cpu->high_ram));
        component_free(&(cpu->high_ram));
        cpu->bus=NULL; // remove cpu from bus
    }
}

//=========================================================================
/**
 * @brief Executes an instruction
 * @param lu instruction
 * @param cpu, the CPU which shall execute
 * @return error code
 *
 * See opcode.h and cpu.h
 */
static int cpu_dispatch(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(lu);
    M_REQUIRE_NON_NULL(cpu);

    // Reset ALU values
    cpu->alu.value=0;
    cpu->alu.flags=0;

    switch (lu->family) {

    // ALU
    case ADD_A_HLR:
    case ADD_A_N8:
    case ADD_A_R8:
    case INC_HLR:
    case INC_R8:
    case ADD_HL_R16SP:
    case INC_R16SP:
    case SUB_A_HLR:
    case SUB_A_N8:
    case SUB_A_R8:
    case DEC_HLR:
    case DEC_R8:
    case DEC_R16SP:
    case AND_A_HLR:
    case AND_A_N8:
    case AND_A_R8:
    case OR_A_HLR:
    case OR_A_N8:
    case OR_A_R8:
    case XOR_A_HLR:
    case XOR_A_N8:
    case XOR_A_R8:
    case CPL:
    case CP_A_HLR:
    case CP_A_N8:
    case CP_A_R8:
    case SLA_HLR:
    case SLA_R8:
    case SRA_HLR:
    case SRA_R8:
    case SRL_HLR:
    case SRL_R8:
    case ROTCA:
    case ROTA:
    case ROTC_HLR:
    case ROT_HLR:
    case ROTC_R8:
    case ROT_R8:
    case SWAP_HLR:
    case SWAP_R8:
    case BIT_U3_HLR:
    case BIT_U3_R8:
    case CHG_U3_HLR:
    case CHG_U3_R8:
    case LD_HLSP_S8:
    case DAA:
    case SCCF:
        M_EXIT_IF_ERR(cpu_dispatch_alu(lu, cpu));
        break;

    // STORAGE
    case LD_A_BCR:
    case LD_A_CR:
    case LD_A_DER:
    case LD_A_HLRU:
    case LD_A_N16R:
    case LD_A_N8R:
    case LD_BCR_A:
    case LD_CR_A:
    case LD_DER_A:
    case LD_HLRU_A:
    case LD_HLR_N8:
    case LD_HLR_R8:
    case LD_N16R_A:
    case LD_N16R_SP:
    case LD_N8R_A:
    case LD_R16SP_N16:
    case LD_R8_HLR:
    case LD_R8_N8:
    case LD_R8_R8:
    case LD_SP_HL:
    case POP_R16:
    case PUSH_R16:
        M_EXIT_IF_ERR(cpu_dispatch_storage(lu, cpu));
        break;

    // JUMP
    case JP_CC_N16:
        if(test_cc(extract_cc(lu->opcode), cpu->F)) {
            cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
            cpu->idle_time+=lu->xtra_cycles;
        }
        break;

    case JP_HL:
        cpu->PC = cpu_HL_get(cpu) - lu->bytes;
        break;

    case JP_N16:
        cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
        break;

    case JR_CC_E8:
        if(test_cc(extract_cc(lu->opcode), cpu->F)) {
            cpu->PC += ((int8_t)cpu_read_data_after_opcode(cpu));
            cpu->idle_time+=lu->xtra_cycles;
        }
        break;

    case JR_E8:
        cpu->PC += ((int8_t)cpu_read_data_after_opcode(cpu));
        break;


    // CALLS
    case CALL_CC_N16:
        if(test_cc(extract_cc(lu->opcode), cpu->F)) {
            cpu_SP_push(cpu, cpu->PC + lu->bytes);
            cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
            cpu->idle_time+=lu->xtra_cycles;
        }
        break;

    case CALL_N16:
        cpu_SP_push(cpu, cpu->PC + lu->bytes);
        cpu->PC = cpu_read_addr_after_opcode(cpu) - lu->bytes;
        break;


    // RETURN (from call)
    case RET:
        cpu->PC = cpu_SP_pop(cpu) - lu->bytes;
        break;

    case RET_CC:
        if(test_cc(extract_cc(lu->opcode), cpu->F)) {
            cpu->PC = cpu_SP_pop(cpu) - lu->bytes;
            cpu->idle_time+=lu->xtra_cycles;
        }
        break;

    case RST_U3:
        cpu_SP_push(cpu, cpu->PC + lu->bytes);
        cpu->PC = extract_n3(lu->opcode)*8 - lu->bytes;
        break;


    // INTERRUPT & MISC.
    case EDI:
        cpu->IME = extract_ime(lu->opcode);
        break;

    case RETI:
        cpu->IME = 1; // because always activated
        cpu->PC = cpu_SP_pop(cpu) - lu->bytes;
        break;

    case HALT:
        cpu->HALT=1;
        break;

    case STOP:
    case NOP:
        // Do nothing
        break;

    default: {
        fprintf(stderr, "Unknown instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
    } break;

    } // switch

    cpu->PC+=lu->bytes; // adjust program counter for next instruction
    cpu->idle_time+=lu->cycles-1; // adjust cpu cycles until next instruction

    return ERR_NONE;
}

// See cpu.h
int test_cc(int cc, flags_t flags)
{
    switch(cc) {
    case 0:
        return !get_Z(flags);
    case 1:
        return (get_Z(flags) == FLAG_Z);
    case 2:
        return !get_C(flags);
    case 3:
        return (get_C(flags) == FLAG_C);
    default:
        return 0;
    }
}

// ----------------------------------------------------------------------
//See cpu.h
static int cpu_do_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    // Interruption handling
    int i = IF_IE_compare(cpu);
    if(cpu->IME && i!=-1) {
        cpu->IME=0; // deactivate further interruptions
        bit_unset(&(cpu->IF),i); // set current interruption as in work
        cpu_SP_push(cpu,cpu->PC); // save current program counter
        cpu->PC=INTERRUPTION_START+8*i; // set program counter to treat interruption
        cpu->idle_time+=INTERRUPTION_CYCLES; // give time to treat interruption
    } else {
        opcode_t next_opcode = cpu_read_at_idx(cpu,cpu->PC); // read next opcode

        if(next_opcode==PREFIXED) { // check if prefixed or direct instruction
            cpu_dispatch(&(instruction_prefixed[cpu_read_data_after_opcode(cpu)]),cpu);
        } else {
            cpu_dispatch(&(instruction_direct[next_opcode]),cpu);
        }
    }

    return ERR_NONE;
}

// ======================================================================
//See cpu.h
int cpu_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);

    cpu->write_listener=0;

    if(cpu->idle_time==0) {

        if(cpu->HALT) { // cpu stopped
            if(IF_IE_compare(cpu)!=-1) {
                cpu->HALT=0;
                cpu_do_cycle(cpu);
            }
        } else { // cpu running
            cpu_do_cycle(cpu);
        }

    } else {
        --(cpu->idle_time); // decrement idle time until next instruction
    }

    return ERR_NONE;
}

//See cpu.h
void cpu_request_interrupt(cpu_t* cpu, interrupt_t i)
{
    bit_set(&(cpu->IF),i);
}

//See cpu.h
int IF_IE_compare(cpu_t* cpu)
{
    for(int i=0; i<IREG_SIZE; ++i) { // go from lower to higher bits because of priority
        if(bit_get(cpu->IF & cpu->IE,i)==1) {
            return i;
        }
    }

    return -1;
}
