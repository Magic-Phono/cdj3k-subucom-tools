// SPDX-License-Identifier: GPL-2.0
/*
 * subucom_uinput.c
 * 
 * This file is part of the Magic Phono project (https://magicphono.org/).
 * Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "lib/uinput.h"
#include "lib/subucom.h"
#include "lib/keymap.h"
#include "lib/doom_keymap.h"

#include <linux/input.h>
#include <linux/uinput.h>


#define SCAN_TIME_MS            2

int loop;
void trap(int signal){ loop = 0; }


int main(int argc, char *argv[]) {
    subucom_t subucom;
    uinput_t uinput;
    int ret;

    void fire_input_event(int type, int code, int val) {
        uinput_emit(&uinput, type, code, val);    
    }

    char *device_path = NULL;
    if (argc == 2) {
        device_path = argv[1];
    }

    printf("subucom_uinput: starting...\n");

    keymap_t *keymap = keymap_make();

    ret = uinput_init(&uinput, keymap);
    if (ret != 0) {
        exit(-1);
    }

    ret = subucom_init(&subucom, device_path);
    if (ret != 0) {
        exit(-1);
    }

    subucom_register_keymap(&subucom, keymap, fire_input_event);

    subucom_start_timer(&subucom, SCAN_TIME_MS);

    signal(SIGINT, &trap);

    loop = 1;
    while (loop) {
        subucom_read(&subucom);
    }

    signal(SIGINT, SIG_DFL);

    printf("subucom: tearing down...\n");

    subucom_stop_timer(&subucom);
    subucom_deinit(&subucom);
    uinput_deinit(&uinput);
    keymap_free(keymap);

    return 0;
}


