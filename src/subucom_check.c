// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K utility program that checks for magic key press combo and writes
 *  output to file.
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib/subucom.h"

static void write_magic_file()
{
    FILE *fp = fopen("/tmp/testmode", "w");
    if (fp != NULL)
    {
        fputs("package_usb", fp);
        fclose(fp);
    }
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

    int bytes_read = subucom_read(&subucom);
    uint8_t* buf = subucom._buf;

    if (bytes_read > 0) {
        /* check for TRACK_FWD and LOOP_IN */
        if (buf[5] == 0x10 && buf[7] == 0x01) { 
            printf("subucom_check: magic key combo detected!\n");
            write_magic_file();
            return 1;
        }
    }

    subucom_deinit(&subucom);

    return 0;
}
