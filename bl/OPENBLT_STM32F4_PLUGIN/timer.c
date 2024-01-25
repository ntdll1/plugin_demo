/************************************************************************************//**
* \file         Source/ARMCM4_STM32F4/timer.c
* \brief        Bootloader timer driver source file.
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
#include "../plugin/plugin.h"

/************************************************************************************//**
** \brief     Initializes the polling based millisecond timer driver.
** \attention To keep the ROM footprint low, this function aims to only use LL driver
**            inline functions.
** \return    none.
**
****************************************************************************************/
void TimerInit(void)
{
} /*** end of TimerInit ***/


/************************************************************************************//**
** \brief     Reset the timer by placing the timer back into it's default reset
**            configuration.
** \return    none.
**
****************************************************************************************/
void TimerReset(void)
{
} /* end of TimerReset */


/************************************************************************************//**
** \brief     Updates the millisecond timer.
** \return    none.
**
****************************************************************************************/
void TimerUpdate(void)
{
} /*** end of TimerUpdate ***/


/************************************************************************************//**
** \brief     Obtains the counter value of the millisecond timer.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
blt_int32u TimerGet(void)
{
  return rom_interface->get_tick();
} /*** end of TimerGet ***/


/************************************************************************************//**
** \brief     Override for the HAL driver's GetTick() functionality. This is needed
**            because the bootloader doesn't use interrupts, but the HAL's tick
**            functionality assumes that it does. This will cause the HAL_Delay()
**            function to not work properly. As a result of this override, the HAL's
**            tick functionality works in polling mode.
** \return    Current value of the millisecond timer.
**
****************************************************************************************/
uint32_t HAL_GetTick(void)
{
  /* Link to the bootloader's 1ms timer. */
  return TimerGet();
} /*** end of HAL_GetTick ***/


/*********************************** end of timer.c ************************************/
