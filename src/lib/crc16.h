// SPDX-License-Identifier: GPL-2.0
/*
 *  CRC-16/X-25 for CDJ3K subucom interface
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <stdio.h>
#include <stdint.h>

uint16_t crc16_x25_calc(const void *buf, size_t len);
