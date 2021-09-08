#include "timer.h"

//placed here to prevent an include loop
#include "cpu-storage.h"

// See timer.h
int timer_init(gbtimer_t* timer, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(cpu);

    timer->cpu = cpu;
    timer->counter = 0;

    return ERR_NONE;
}

// See timer.h
int timer_cycle(gbtimer_t* timer)
{
    M_REQUIRE_NON_NULL(timer);

    bit_t old_state = timer_state(timer);

    timer->counter += TIMER_CYCLE;
    int ret = cpu_write_at_idx(timer->cpu, REG_DIV, msb8(timer->counter)); // sync 8 strong bits of timer with bus
    if(ret!=ERR_NONE) {
        return ret;
    }

    timer_incr_if_state_change(timer, old_state);

    return ERR_NONE;
}

// See timer.h
int timer_bus_listener(gbtimer_t* timer, addr_t addr)
{
    M_REQUIRE_NON_NULL(timer);

    bit_t old_state = timer_state(timer);

    if(addr == REG_DIV) {
        timer->counter = 0;
        int ret = cpu_write_at_idx(timer->cpu, REG_DIV, 0); // sync 8 strong bits of timer with bus
        if(ret!=ERR_NONE) {
            return ret;
        }
        timer_incr_if_state_change(timer, old_state);
    } else if (addr == REG_TAC) {
        timer_incr_if_state_change(timer, old_state);
    }

    return ERR_NONE;
}

// See timer.h
bit_t timer_state(gbtimer_t* timer)
{
    M_REQUIRE_NON_NULL(timer);

    bit_t tac_2 = (bit_get(cpu_read_at_idx(timer->cpu, REG_TAC), 2));

    bit_t tac_index = cpu_read_at_idx(timer->cpu, REG_TAC) & 0x03;

    switch(tac_index) {
    case 0 : {
        return tac_2 & bit_get(msb8(timer->counter), 1); //9th bit is 1st of most significant bits
    }
    case 1 : {
        return tac_2 & bit_get(timer->counter, 3);
    }
    case 2 : {
        return tac_2 & bit_get(timer->counter, 5);
    }
    case 3 : {
        return tac_2 & bit_get(timer->counter, 7);
    }
    default:
        return 0;
    }
}

// See timer.h
void timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state)
{
    if(timer!=NULL && old_state == 1 && timer_state(timer) == 0)  {
        uint8_t timer_val = cpu_read_at_idx(timer->cpu, REG_TIMA);
        if(timer_val == 0xFF) {
            cpu_request_interrupt(timer->cpu,TIMER); // raise interruption when "go-around"
            uint8_t reset_val = cpu_read_at_idx(timer->cpu, REG_TMA);
            cpu_write_at_idx(timer->cpu, REG_TIMA, reset_val);
        } else {
            cpu_write_at_idx(timer->cpu, REG_TIMA, ++timer_val);
        }
    }
}
