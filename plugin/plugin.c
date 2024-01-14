#include "../plugin.h"

void plugin_init(void)
{
  
}

void plugin_loop(void)
{
  uint32_t tick = ROM_INTERFACE->get_tick();
  if (tick >= 5000) {
    ROM_INTERFACE->write_reg(0x1ed, 500);
  }
}

void plugin_handle_serial(void)
{
  
}
