// SPDX-License-Identifier: GPL-2.0
/*
 *  DOOM keymap for CDJ3K subucom
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <linux/input.h>
#include <linux/uinput.h>
#include <stdlib.h>
#include <stdio.h>

#include "subucom.h"

#define NUM_BUTTONS 10
button_def_t buttons[NUM_BUTTONS] = {
    {.id = ROTARY_BUTTON, .byte = 0x0B, .bit = 0x10, .keycode = KEY_ENTER},
    {.id = BACK,          .byte = 0x0B, .bit = 0x01, .keycode = KEY_BACKSPACE},
    {.id = CUE,           .byte = 0x05, .bit = 0x02, .keycode = 0},
    {.id = PLAY,          .byte = 0x05, .bit = 0x01, .keycode = 0},
    {.id = BEAT_JUMP_REV, .byte = 0x05, .bit = 0x80, .keycode = KEY_LEFTCTRL},
    {.id = BEAT_JUMP_FWD, .byte = 0x05, .bit = 0x40, .keycode = KEY_SPACE},
    {.id = TRACK_REV,     .byte = 0x05, .bit = 0x20, .keycode = KEY_COMMA},
    {.id = TRACK_FWD,     .byte = 0x05, .bit = 0x10, .keycode = KEY_DOT},
    {.id = BEAT_LOOP_4,   .byte = 0x07, .bit = 0x10, .keycode = KEY_LEFTALT},
    {.id = BEAT_LOOP_8,   .byte = 0x07, .bit = 0x20, .keycode = 0}
};

#define NUM_SLIP_STATES 3
selector_state_t slip_states[NUM_SLIP_STATES] = {
    {.id = FWD,      .value = 0x03, .as_button = false, .keycode = 0},
    {.id = SLIP_REV, .value = 0x02, .as_button = true, .keycode = KEY_KP8},
    {.id = REVERSE,  .value = 0x01, .as_button = true, .keycode = KEY_KP2}
};

#define NUM_SELECTORS 1
selector_def_t selectors[NUM_SELECTORS] = {
    {.id = SLIP, .byte = 0x04, .state_count = 3, .states = slip_states},
};

#define NUM_ENCODERS 2
encoder_def_t encoders[NUM_ENCODERS] = {
    {
        .id = ROTARY,      
        .byte = 0x0E,
        .dir_as_button = true, 
        .left_id = ROTARY_LEFT,
        .right_id = ROTARY_RIGHT,
        .left_keycode = KEY_UP,
        .right_keycode = KEY_DOWN
    },
    {
        .id = ENCODER_JOG,
        .byte = 0x1C,
        .dir_as_button = false,
        .left_id = JOG_LEFT,   
        .right_id = JOG_RIGHT,
        .left_keycode = 0,
        .right_keycode = 0
    }
};

#define NUM_JOGS 1
jog_def_t jogs[NUM_JOGS] = {
    {
        .id = JOG,
        .byte = 0x1E,
        .dir_as_button = true,
        .button_keycode = KEY_LEFTCTRL,
        .left_keycode = KEY_KP4,
        .right_keycode = KEY_KP6
    },
};

keymap_t* keymap_make() {
    keymap_t* keymap = (keymap_t *)calloc(1, sizeof(keymap_t));

    keymap->buttons = buttons;
    keymap->slip_states = slip_states;
    keymap->selectors = selectors;
    keymap->encoders = encoders;
    keymap->jogs = jogs;

    keymap->num_buttons = NUM_BUTTONS;
    keymap->num_slip_states = NUM_SLIP_STATES;
    keymap->num_selectors = NUM_SELECTORS;
    keymap->num_encoders = NUM_ENCODERS;
    keymap->num_jogs = NUM_JOGS;

    if (keymap == NULL) {
        printf("doom_keymap: Could not allocate keymap!\n");
        return NULL;
    }

    return keymap;
}

int keymap_register_uinput_keycodes(keymap_t* keymap, int uinput_fd) {
    uint32_t count = 0;

    for (int i=0 ; i<keymap->num_buttons ; i++) {
        uint32_t keycode = keymap->buttons[i].keycode;
        if (keycode != 0) {
            ioctl(uinput_fd, UI_SET_KEYBIT, keycode);
            count++;
        }
    }

    return count;
}

void keymap_free(keymap_t* keymap) {
    free(keymap);
}
