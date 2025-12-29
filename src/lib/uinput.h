// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K uinput driver
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#ifndef __SUBUCOM_UINPUT_H_
#define __SUBUCOM_UINPUT_H_

#include "keymap.h"

typedef struct uinput {
	int				fd;
} uinput_t;

void uinput_emit(uinput_t* uinput, int type, int code, int val);
int  uinput_init(uinput_t* uinput, keymap_t* keymap);
void uinput_deinit(uinput_t* uinput);

#endif        /* __SUBUCOM_UINPUT_H_ */
