#include <stdint.h>

typedef struct
{
    uint32_t (*get_tick)(void);
    int (*send_uart)(uint8_t *, uint16_t);
    int (*send_can)(uint16_t, uint8_t *, uint8_t);
} rom_interface_t;

extern rom_interface_t *rom_interface;

typedef struct
{
    void (*entry)(void);
    void (*loop)(void);
    int (*handle_uart)(uint8_t *, uint16_t);
    int (*handle_can)(uint16_t, uint8_t *, uint8_t);
} plugin_interface_t;

extern plugin_interface_t *plugin_interface;
/* bl interface just wraps plugin interface, so they have the same definations */
extern plugin_interface_t *bl_interface;
