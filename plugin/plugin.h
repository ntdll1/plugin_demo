#include <stdint.h>
#include <stdlib.h>

#define ROM_INTERFACE_ADDR 0x8005F80
#define PLIGIN_INTERFACE_ADDR 0x8006000

struct rom_interface
{
    uint32_t (*get_tick)(void);
    int (*read_reg)(int);
    int (*write_reg)(int, int);
    int (*send_serial)(void *, size_t);
};
#define ROM_INTERFACE ((struct rom_interface *)ROM_INTERFACE_ADDR)

struct plugin_interface
{
    void (*entry)(void);
    void (*loop)(void);
    int (*handle_serial)(void *, size_t);
};
#define PLUGIN_INTERFACE ((struct plugin_interface *)PLIGIN_INTERFACE_ADDR)
