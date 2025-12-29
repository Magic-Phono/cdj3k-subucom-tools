// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K utility program to reset read timer.
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/subucom.h"

int main(int argc, char *argv[]) {
    subucom_t subucom;
    int ret;

    char *device_path = NULL;
    if (argc == 2) {
        device_path = argv[1];
    }

    ret = subucom_init(&subucom, device_path);
    if (ret != 0) {
        exit(-1);
    }

    if (subucom_is_timer_running(&subucom)) {
        int timer_value = subucom_read_timer_interval(&subucom);
        printf("subucom: timer is currently running with interval %d, stopping...\n", timer_value);
        subucom_stop_timer(&subucom);
    } else {
        printf("subucom: timer is not running.\n");
    }

    subucom_deinit(&subucom);

    return 0;
}
