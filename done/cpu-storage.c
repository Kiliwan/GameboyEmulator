/**
 * @file cpu-registers.c
 * @brief Game Boy CPU simulation, register part
 *
 * @date 2019
 */
#include "cpu-storage.h"

// See cpu-storage.h
data_t cpu_read_at_idx(const cpu_t* cpu, addr_t addr)
{
    if(cpu == NULL) {
        return -1;
    }

    data_t data = 0;
    bus_read(*(cpu->bus),addr,&data);

    return data;
}

// See cpu-storage.h
addr_t cpu_read16_at_idx(const cpu_t* cpu, addr_t addr)
{
    if(cpu == NULL) {
        return -1;
    }
    addr_t data16 = 0;
    bus_read16(*(cpu->bus),addr,&data16);

    return data16;
}

// See cpu-storage.h
int cpu_write_at_idx(cpu_t* cpu, addr_t addr, data_t data)
{
    if(cpu==NULL || cpu->bus==NULL) {
        return ERR_BAD_PARAMETER;
    }
    cpu->write_listener=addr; // for the listeners
    return bus_write(*(cpu->bus),addr,data);
}

// See cpu-storage.h
int cpu_write16_at_idx(cpu_t* cpu, addr_t addr, addr_t data16)
{
    if(cpu==NULL || cpu->bus==NULL) {
        return ERR_BAD_PARAMETER;
    }
    cpu->write_listener=addr; // for the listeners
    return bus_write16(*(cpu->bus),addr,data16);
}

// See cpu-storage.h
int cpu_SP_push(cpu_t* cpu, addr_t data16)
{
    if(cpu==NULL) {
        return ERR_BAD_PARAMETER;
    }
    if(cpu->SP>=2) {
        cpu->SP-=2; // else unspecified behavior, so do nothing
    }
    return cpu_write16_at_idx(cpu, cpu->SP, data16);
}

// See cpu-storage.h
addr_t cpu_SP_pop(cpu_t* cpu)
{
    if(cpu==NULL) {
        return ERR_BAD_PARAMETER;
    }

    addr_t data16 = cpu_read16_at_idx(cpu, cpu->SP);
    cpu->SP+=2;
    return data16;

}

// See cpu-storage.h
int cpu_dispatch_storage(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    switch (lu->family) {
    case LD_A_BCR:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_BC_get(cpu)));
        break;

    case LD_A_CR:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, REGISTERS_START+cpu_reg_get(cpu, REG_C_CODE)));
        break;

    case LD_A_DER:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_DE_get(cpu)));
        break;

    case LD_A_HLRU:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_HL(cpu));
        cpu_HL_set(cpu, cpu_HL_get(cpu)+extract_HL_increment(lu->opcode));
        break;

    case LD_A_N16R:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, cpu_read_addr_after_opcode(cpu)));
        break;

    case LD_A_N8R:
        cpu_reg_set(cpu, REG_A_CODE, cpu_read_at_idx(cpu, REGISTERS_START+cpu_read_data_after_opcode(cpu)));
        break;

    case LD_BCR_A:
        cpu_write_at_idx(cpu, cpu_BC_get(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_CR_A:
        cpu_write_at_idx(cpu, REGISTERS_START+cpu_reg_get(cpu, REG_C_CODE), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_DER_A:
        cpu_write_at_idx(cpu, cpu_DE_get(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_HLRU_A:
        cpu_write_at_HL(cpu, cpu_reg_get(cpu, REG_A_CODE));
        cpu_HL_set(cpu, cpu_HL_get(cpu)+extract_HL_increment(lu->opcode));
        break;

    case LD_HLR_N8:
        cpu_write_at_HL(cpu, cpu_read_data_after_opcode(cpu));
        break;

    case LD_HLR_R8:
        cpu_write_at_HL(cpu, cpu_reg_get(cpu, extract_reg(lu->opcode,0)));
        break;

    case LD_N16R_A:
        cpu_write_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_N16R_SP:
        cpu_write16_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu->SP);
        break;

    case LD_N8R_A:
        cpu_write_at_idx(cpu, REGISTERS_START+cpu_read_data_after_opcode(cpu), cpu_reg_get(cpu, REG_A_CODE));
        break;

    case LD_R16SP_N16:
        cpu_reg_pair_SP_set(cpu, extract_reg_pair(lu->opcode), cpu_read_addr_after_opcode(cpu)); // Does for other registers if not AF code
        break;

    case LD_R8_HLR:
        cpu_reg_set(cpu, extract_reg(lu->opcode,3), cpu_read_at_HL(cpu));
        break;

    case LD_R8_N8:
        cpu_reg_set(cpu, extract_reg(lu->opcode,3), cpu_read_data_after_opcode(cpu));
        break;

    case LD_R8_R8:
        cpu_reg_set(cpu, extract_reg(lu->opcode,3), cpu_reg_get(cpu, extract_reg(lu->opcode,0)));
        break;

    case LD_SP_HL:
        cpu->SP=cpu->HL;
        break;

    case POP_R16:
        cpu_reg_pair_set(cpu, extract_reg_pair(lu->opcode), cpu_SP_pop(cpu));
        break;

    case PUSH_R16:
        cpu_SP_push(cpu, cpu_reg_pair_get(cpu, extract_reg_pair(lu->opcode)));
        break;

    default:
        fprintf(stderr, "Unknown STORAGE instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
        break;
    } // switch

    return ERR_NONE;
}
