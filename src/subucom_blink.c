// SPDX-License-Identifier: GPL-2.0
/*
 *  subucom_blink.c
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "lib/subucom.h"

int loop;
void trap(int signal){ loop = 0; }

#define SUBUCOM_BUFSIZE_LED SUBUCOM_BUFSIZE-2

uint8_t buf_on[SUBUCOM_BUFSIZE_LED];
uint8_t buf_off[SUBUCOM_BUFSIZE_LED];

static void setup_buffers() {
    memset(buf_on, 0x00, SUBUCOM_BUFSIZE_LED);
    memset(buf_off, 0x00, SUBUCOM_BUFSIZE_LED);

    buf_on[0x02] = 0x3F;
    buf_on[0x03] = 0xFF;
    buf_on[0x04] = 0xFF;
    buf_on[0x05] = 0xCF;
    buf_on[0x06] = 0x00;
    buf_on[0x07] = 0xFB;
    buf_on[0x08] = 0xDB;
    buf_on[0x09] = 0x15;
    buf_on[0x0a] = 0x10;
    buf_on[0x0b] = 0x00;
    buf_on[0x0c] = 0x44;
    buf_on[0x0d] = 0x78;
    buf_on[0x0e] = 0x7F;
    buf_on[0x0f] = 0x44;
    buf_on[0x10] = 0x78;
    buf_on[0x11] = 0x7F;
    buf_on[0x12] = 0x44;
    buf_on[0x13] = 0x78;
    buf_on[0x14] = 0x7F;
    buf_on[0x15] = 0x44;
    buf_on[0x16] = 0x78;
    buf_on[0x17] = 0x7F;
    buf_on[0x18] = 0x44;
    buf_on[0x19] = 0x78;
    buf_on[0x1a] = 0x7F;
    buf_on[0x1b] = 0x44;
    buf_on[0x1c] = 0x78;
    buf_on[0x1d] = 0x7F;
    buf_on[0x1e] = 0x44;
    buf_on[0x1f] = 0x78;
    buf_on[0x20] = 0x7F;
    buf_on[0x21] = 0x44;
    buf_on[0x22] = 0x78;
    buf_on[0x23] = 0x7F;
    buf_on[0x24] = 0x88;
    buf_on[0x25] = 0xF0;
    buf_on[0x26] = 0xFF;
    buf_on[0x27] = 0x88;
    buf_on[0x28] = 0xF0;
    buf_on[0x29] = 0xFF;
    buf_on[0x2a] = 0x88;
    buf_on[0x2b] = 0xF0;
    buf_on[0x2c] = 0xFF;
}

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

    setup_buffers();

    signal(SIGINT, &trap);

    loop = 1;
    
    int8_t bit_mask = 0x0;

    int8_t buf_idx = 0x02;
    int8_t bit_idx = 0;

    uint8_t *buf = buf_off;
    while (loop) {      
        // uint8_t *buf = (buf_idx % 2 == 0) ? buf_on :  buf_off;

        uint8_t c = buf[buf_idx];

        // buf[buf_idx] = (1 << bit_idx);
        // buf[buf_idx] = c | (1 << bit_idx);
        buf[buf_idx] = c+1;
        

        printf("%02x-%02x -- ", buf_idx, bit_idx);

        // write

        subucom_write(&subucom, buf, SUBUCOM_BUFSIZE_LED);

        bit_idx++;

        if (bit_idx == 8) {
             buf[buf_idx] = 0;
            bit_idx = 0;
            buf_idx++;
        }

        if (buf_idx > 0x2c) {
            break;
        }

        // dump
        for (size_t i = 0; i < (size_t)SUBUCOM_BUFSIZE_LED; i++) {
            printf("%02x ", buf[i]);
        }
        printf("\n");


        getchar();
        

        // usleep(500000);
    }

    signal(SIGINT, SIG_DFL);

    subucom_deinit(&subucom);

    return 0;
}
