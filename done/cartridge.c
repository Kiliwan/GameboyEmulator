#include "cartridge.h"

// See cartridge.h
int cartridge_init_from_file(component_t* c, const char* filename)
{
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(filename);

    FILE* input;

    input = fopen(filename, "rb"); // open in binary reading mode

    if(input == NULL) {
        return ERR_IO;
    }

    // assign cartridge data to memory
    size_t count = fread(c->mem->memory, sizeof(data_t), BANK_ROM_SIZE, input);

    if(count!=BANK_ROM_SIZE || c->mem->memory[CARTRIDGE_TYPE_ADDR] != 0) {
        fclose(input);
        return ERR_BAD_PARAMETER;
    }

    int ret = fclose(input);
    if(ret!=ERR_NONE) {
        return ERR_IO;
    }

    return ERR_NONE;
}

// See cartridge.h
int cartridge_init(cartridge_t* ct, const char* filename)
{
    M_REQUIRE_NON_NULL(ct);
    M_REQUIRE_NON_NULL(filename);

    int create = component_create(&(ct->c), BANK_ROM_SIZE);
    if(create != ERR_NONE) {
        return ERR_BAD_PARAMETER;
    }

    int err_code = cartridge_init_from_file(&(ct->c), filename);

    if(err_code == ERR_BAD_PARAMETER) {
        mem_free(ct->c.mem);
    }


    return err_code;
}

// See cartridge.h
int cartridge_plug(cartridge_t* ct, bus_t bus)
{
    M_REQUIRE_NON_NULL(ct);

    return bus_forced_plug(bus, &(ct->c), BANK_ROM0_START, BANK_ROM1_END, 0);
}

// See cartridge.h
void cartridge_free(cartridge_t* ct)
{
    if(ct != NULL) {
        component_free(&(ct->c));
    }
}
