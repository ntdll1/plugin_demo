#include "plugin.h"

extern void plugin_entry(void);

void plugin_init(void)
{
}

static int sent = 0;

void plugin_loop(void)
{
  uint32_t tick = rom_interface->get_tick();
  if (!sent && tick >= 5000) {
    sent = 1;
    uint16_t val = 50;
    rom_interface->send_can(0x1ed, (uint8_t *)&val, sizeof(val));
  }
}

int plugin_handle_uart(uint8_t *buf, uint16_t len)
{
  return 0;
}

int plugin_handle_can(uint16_t id, uint8_t *buf, uint8_t dlc)
{
  return 0;
}

const void *plugin_interface_table[] __attribute__((section(".plugin_interface"))) = {
  plugin_entry,
  plugin_loop,
  plugin_handle_uart,
  plugin_handle_can,
};
