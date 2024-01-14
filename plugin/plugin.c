#include "plugin.h"

void plugin_init(void)
{
  
}

static uint32_t tick = 0;

void plugin_handle_timer(void)
{
  ++ tick;
}

void plugin_loop(void)
{
  // 为了演示新接口功能，这里自己计时而不是用get_tick获取时间
  if (tick >= 5000) {
    ROM_INTERFACE->write_reg(0x1ed, 500);
  }
}

void plugin_handle_serial(void)
{
  
}
