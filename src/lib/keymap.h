// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K subucom keymap header
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

 #ifndef __KEYMAP_H_
#define __KEYMAP_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum button_type {
    ROTARY_BUTTON,
    BACK,
    CUE,
    PLAY,
    BEAT_JUMP_REV,
    BEAT_JUMP_FWD,
    TRACK_REV,
    TRACK_FWD,
    BEAT_LOOP_4,
    BEAT_LOOP_8
} button_type_t;

typedef enum encoder_type {
    ROTARY,
    ENCODER_JOG
} encoder_type_t;

typedef enum encoder_dir_type {
    ROTARY_LEFT,
    ROTARY_RIGHT,
    JOG_LEFT,
    JOG_RIGHT
} encoder_dir_type_t;

typedef enum selector_type {
    SLIP
} selector_type_t;

typedef enum selector_states {
    FWD,
    SLIP_REV,
    REVERSE
} selector_states_t;

typedef enum jog_type {
    JOG
} jog_type_t;


typedef struct button_def {
    button_type_t id;
    uint8_t byte;
    uint8_t bit;
    uint32_t keycode;
} button_def_t;

typedef struct selector_state {
    selector_states_t id;
    uint8_t value;
    bool as_button;
    int keycode;
} selector_state_t;

typedef struct selector_def {
    selector_type_t id;
    uint8_t byte;
    uint8_t state_count;
    selector_state_t *states;
} selector_def_t;

typedef struct encoder_def {
    encoder_type_t id;
    uint8_t byte; /* note: some encoders are 2-bytes */
    bool dir_as_button;
    encoder_dir_type_t left_id;
    encoder_dir_type_t right_id;
    int left_keycode;
    int right_keycode;
} encoder_def_t;

typedef struct jog_def {
    jog_type_t id;
    uint8_t byte;
    bool dir_as_button;
    int button_keycode;
    int left_keycode;
    int right_keycode;
} jog_def_t;

typedef struct keymap {
    button_def_t *buttons;
    selector_state_t *slip_states;
    selector_def_t *selectors;
    encoder_def_t *encoders;
    jog_def_t *jogs;

    uint8_t num_buttons;
    uint8_t num_slip_states;
    uint8_t num_selectors;
    uint8_t num_encoders;
    uint8_t num_jogs;
} keymap_t;

keymap_t* keymap_make();
int       keymap_register_uinput_keycodes(keymap_t* keymap, int uinput_fd);
void      keymap_free(keymap_t* keymap);

#endif // __KEYMAP_H_