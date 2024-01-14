#include "../plugin/plugin.h"

#include "stm32f1xx.h"
#include "led.h"

int rom_get_tick() {
    return HAL_GetTick();
}

int rom_read_reg(int addr) {
    return 0;
}

int rom_write_reg(int addr, int value) {
    if (addr == 0x1ed) {
        LedSetInterval(value);
    }
    return 0;
}

int rom_send_serial(void *buf, int size) {
    return 0;
}

void plugin_stub() {
}