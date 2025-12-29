// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K utility program that reads subucom controller state and dumps to the
 *  screen continously.
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <curses.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "lib/crc16.h"
#include "lib/subucom.h"


#define SCAN_TIME_MS            50

enum colors {
    COLOR_CHANGE = 1,
    COLOR_ON = 2,
    COLOR_OK = 3,
    COLOR_FAIL = 4
};

int loop;
void trap(int signal){ loop = 0; }

int main(int argc, char *argv[]) {
    subucom_t subucom;
    uint8_t starting_buf[SUBUCOM_BUFSIZE] = {0};
    uint8_t prev_buf[SUBUCOM_BUFSIZE] = {0};
    int ret;

    char *device_path = NULL;
    if (argc == 2) {
        device_path = argv[1];
    }

    initscr();
    start_color();
    init_pair(COLOR_CHANGE, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_ON, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_OK, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_FAIL, COLOR_RED, COLOR_BLACK);
    noecho();
    clear();
    
    ret = subucom_init(&subucom, device_path);
    if (ret != 0) {
        exit(-1);
    }

    subucom_start_timer(&subucom, SCAN_TIME_MS);

    for (size_t i = 0; i < SUBUCOM_BUFSIZE; i++) {
        printw("%02x ", (unsigned char)i);
    }
    addstr("\n");
    hline(ACS_HLINE, 191);

    signal(SIGINT, &trap);

    subucom_read(&subucom);
    subucom_read(&subucom);
    memcpy(starting_buf, subucom._buf, SUBUCOM_BUFSIZE);

    loop = 1;
    while (loop) {        
        int bytes_read = subucom_read(&subucom);

        move(2, 0);
        clrtoeol(); 

        uint8_t* buf = subucom._buf;

        if (bytes_read > 0) {
            uint16_t crc16 = crc16_x25_calc(buf, SUBUCOM_BUFSIZE-2);

            for (size_t i = 0; i < (size_t)bytes_read; i++) {
                if (buf[i] != prev_buf[i]) {
                    attron(COLOR_PAIR(COLOR_CHANGE) | A_BOLD);
                } else if (buf[i] != starting_buf[i]) {
                    attron(COLOR_PAIR(COLOR_ON) | A_BOLD);
                }
                printw("%02x ", buf[i]);
                attroff(COLOR_PAIR(COLOR_CHANGE) | COLOR_PAIR(COLOR_ON) | A_BOLD);
            }

            printw(" -- ");
            uint8_t crch = (crc16 & 0xFF);
            uint8_t crcl = (crc16 >> 8);
            if (crch == buf[SUBUCOM_BUFSIZE-2] && crcl == buf[SUBUCOM_BUFSIZE-1]) {
                attron(COLOR_PAIR(COLOR_OK) | A_BOLD);
                printw("OK");
                attroff(COLOR_PAIR(COLOR_OK) | A_BOLD);
            } else {
                attron(COLOR_PAIR(COLOR_FAIL) | A_BOLD);
                printw("FAIL");
                attroff(COLOR_PAIR(COLOR_FAIL) | A_BOLD);
            }
            
            addstr("\n");
        } else {
            break;
        }

        refresh();
        memcpy(prev_buf, buf, SUBUCOM_BUFSIZE);

        usleep(SCAN_TIME_MS * 1000);
    }

    signal(SIGINT, SIG_DFL);

    endwin();

    subucom_stop_timer(&subucom);
    subucom_deinit(&subucom);

    return 0;
}


