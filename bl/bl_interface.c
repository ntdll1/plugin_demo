#include "boot.h"
#include "../plugin/plugin.h"

extern void bl_entry(void);

void bl_init(void)
{
  BootInit();
  plugin_interface->entry();
}

void bl_loop(void)
{
  BootTask();
  plugin_interface->loop();
}

int bl_handle_uart(uint8_t *buf, uint16_t len)
{
#if (BOOT_COM_RS232_ENABLE > 0)
  blt_bool OpenBLTHandleUart(blt_int8u *data, blt_int16u len);

  OpenBLTHandleUart(buf, len);
  /* always eat uart data */
  return 1;
#else
  return 0;
#endif
}


int bl_handle_can(uint16_t id, uint8_t *buf, uint8_t dlc)
{
#if (BOOT_COM_CAN_ENABLE > 0)
  blt_bool OpenBLTHandleCan(blt_int16u id, blt_int8u *data, blt_int8u dlc);

  if (OpenBLTHandleCan(id, buf, dlc)) {
    return 1;
  }
#endif
  return plugin_interface->handle_can(id, buf, dlc);
}

__attribute__((section(".plugin_stub"))) void plugin_stub()
{
}

const void *bl_interface_table[] __attribute__((section(".bl_interface"))) = {
  bl_entry,
  bl_loop,
  bl_handle_uart,
  bl_handle_can,
};

const void *plugin_interface_table[] __attribute__((section(".plugin_interface"))) = {
  plugin_stub,
  plugin_stub,
  plugin_stub,
  plugin_stub,
};
