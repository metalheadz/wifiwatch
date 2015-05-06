/**
  ******************************************************************************
  * @file    dmsg.c
  * @author  Baoshi <mail(at)ba0sh1(dot)com>
  * @version 0.1
  * @date    Dec 16, 2014
  * @brief   Some TX only functions for debug output,
  *          meant for use with ESP8266 UART1.
  *
  ******************************************************************************
  * @copyright
  *
  * Copyright (c) 2015, Baoshi Zhu. All rights reserved.
  * Use of this source code is governed by a BSD-style license that can be
  * found in the LICENSE.txt file.
  *
  * THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED
  * WARRANTY.  IN NO EVENT WILL THE AUTHOR(S) BE HELD LIABLE FOR ANY DAMAGES
  * ARISING FROM THE USE OF THIS SOFTWARE.
  *
  */

#include <stdarg.h>
#include "common.h"

void ICACHE_FLASH_ATTR dmsg_init() {}

void ICACHE_FLASH_ATTR dmsg_printf(const char *fmt, ...)
{
  // va_list args;
  // va_start(args, fmt);
  // os_vprintf(fmt, args);
  // va_end(args);
}

void ICACHE_FLASH_ATTR dmsg_putchar(char c)
{
  dmsg_printf("%c", c);
}

void ICACHE_FLASH_ATTR dmsg_puts(const char *s)
{
  dmsg_printf("%s", s);
}


