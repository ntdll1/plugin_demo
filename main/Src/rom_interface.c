#include "../../plugin/plugin.h"
#include "stm32f4xx.h"

extern UART_HandleTypeDef huart2;

int rom_get_tick() {
    return HAL_GetTick();
}

int rom_send_serial(void *buf, int size) {
    return 0;
}

int rom_send_uart(uint8_t *buf, uint16_t len)
{
    return HAL_UART_Transmit(&huart2, buf, len, HAL_MAX_DELAY) == HAL_OK;
}

int rom_send_can(uint16_t id, uint8_t *buf, uint8_t dlc)
{
//   CAN_TxHeaderTypeDef txMsgHeader;
//   uint32_t txMsgMailbox;
//   HAL_StatusTypeDef txStatus;

//   /* configure the message that should be transmitted. */
//   /* set the 11-bit CAN identifier. */
//   txMsgHeader.StdId = id;
//   txMsgHeader.IDE = CAN_ID_STD;

//   txMsgHeader.RTR = CAN_RTR_DATA;
//   txMsgHeader.DLC = dlc;

//   /* submit the message for transmission. */
//   txStatus = HAL_CAN_AddTxMessage(&canHandle, &txMsgHeader, data,
//                                   (uint32_t *)&txMsgMailbox);
  return 0;
}

void plugin_stub() {
}

const void *rom_interface_table[] __attribute__((section(".rom_interface"))) = {
  rom_get_tick,
  rom_send_uart,
  rom_send_can,
};

const void *plugin_interface_table[] __attribute__((section(".plugin_interface"))) = {
  plugin_stub,
  plugin_stub,
  plugin_stub,
  plugin_stub,
};

plugin_interface_t *plugin_interface = (plugin_interface_t *)plugin_interface_table;
/* for main program, bl is just alias of plugin */