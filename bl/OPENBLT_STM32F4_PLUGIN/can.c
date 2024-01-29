/************************************************************************************//**
* \file         Source/ARMCM4_STM32F4/can.c
* \brief        Bootloader CAN communication interface source file.
* \ingroup      Target_ARMCM4_STM32F4
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2013  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/


/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#if (BOOT_COM_CAN_ENABLE > 0)
#include "../plugin/plugin.h"


#define CAN_BUF_SIZE 16

blt_int8u can_queue_buf[CAN_BUF_SIZE][8];
blt_int8u can_queue_dlc[CAN_BUF_SIZE];

int can_queue_head, can_queue_tail;

/************************************************************************************//**
** \brief     Initializes the CAN controller and synchronizes it to the CAN bus.
** \return    none.
**
****************************************************************************************/
void CanInit(void)
{
  can_queue_head = can_queue_tail = 0;
} /*** end of CanInit ***/


blt_bool OpenBLTHandleCan(blt_int16u id, blt_int8u *data, blt_int8u dlc)
{
  /* handle only packet with specific ID, as all packets are with same ID, the priority is also meaningless */
  if (id == BOOT_COM_CAN_RX_MSG_ID) {
    int next_can_queue_tail = can_queue_tail + 1;
    if (next_can_queue_tail == CAN_BUF_SIZE) {
      next_can_queue_tail = 0;
    }
    /* accept packet only if queue not full */
    if (next_can_queue_tail != can_queue_head) {
      can_queue_dlc[can_queue_tail] = dlc;
      *(uint64_t *)(can_queue_buf[can_queue_tail]) = *(uint64_t *)data;
      can_queue_tail = next_can_queue_tail;
      return BLT_TRUE;
    }
  }
  return BLT_FALSE;
}

/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void CanTransmitPacket(blt_int8u *data, blt_int8u len)
{
  rom_interface->send_can(BOOT_COM_CAN_TX_MSG_ID, data, len);
} /*** end of CanTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE is a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool CanReceivePacket(blt_int8u *data, blt_int8u *len)
{
  /* NOTE: no need to disable isr. it is not possible that data/dlc/head get overwritten during this function */
  if (can_queue_head != can_queue_tail) {
    *(uint64_t *)data = *(uint64_t *)(can_queue_buf[can_queue_head]);
    *len = can_queue_dlc[can_queue_head];
    if (++ can_queue_head == CAN_BUF_SIZE) {
      can_queue_head = 0;
    }
  }

  return BLT_FALSE;
} /*** end of CanReceivePacket ***/

#endif /* BOOT_COM_CAN_ENABLE > 0 */


/*********************************** end of can.c **************************************/
