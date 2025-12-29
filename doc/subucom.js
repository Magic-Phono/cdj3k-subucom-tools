//
// CDJ3K SUBUCOM documentation generator
//
// This file is part of the Magic Phono project (https://magicphono.org/).
//Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
//

const render = require('bit-field/lib/render');
const onml = require('onml');
const fs = require('node:fs');

const FONT_SIZE = 10;
const LEFT_MARGIN = 16;
const TOP_MARGIN = 10;

function subucom_input() {
    const reg =[
        // byte 0
        {                          "bits": 8, "type": 1 },

        // byte 1
        {                          "bits": 8, "type": 1 },

        // byte 2
        { "name": "SUBUCOM_MAJOR_REVISION",   "bits": 8 },

        // byte 3
        { "name": "SUBUCOM_MINOR_REVISION",   "bits": 8 },

        // byte 4
        { "name": "SLIP_PADDLE",   "bits": 2 },
        {                          "bits": 6, "type": 1 },

        // byte 5
        { "name": "PLAY",          "bits": 1, "type": 4 },
        { "name": "CUE",           "bits": 1, "type": 4 },
        { "name": "SEARCH_FWD¹",   "bits": 1, "type": 4 },
        { "name": "SEARCH_REV¹",   "bits": 1, "type": 4 },
        { "name": "TRACK_FWD¹",    "bits": 1, "type": 4 },
        { "name": "TRACK_REV¹",    "bits": 1, "type": 4 },
        { "name": "BEATJUMP_FWD",  "bits": 1, "type": 4 },
        { "name": "BEATJUMP_REV",  "bits": 1, "type": 4 },
        
        // byte 6
        { "name": "TEMPO_RESET",   "bits": 1, "type": 4},
        { "name": "TEMPO_MASTER²", "bits": 1, "type": 4 },
        { "name": "TEMPO_RANGE²",  "bits": 1, "type": 4 },
        {                          "bits": 1, "type": 1 },
        { "name": "KEYSYNC²",      "bits": 1, "type": 4 },
        { "name": "BEATSYNC²",     "bits": 1, "type": 4 },
        { "name": "MASTER²",       "bits": 1, "type": 4 },
        {                          "bits": 1, "type": 1 },

        // byte 7s
        { "name": "LOOP_IN",       "bits": 1, "type": 4 },
        { "name": "LOOP_OUT",      "bits": 1, "type": 4 },
        { "name": "RELOOP",        "bits": 1, "type": 4 },
        {                          "bits": 1, "type": 1 },
        { "name": "BEAT_LOOP_4",   "bits": 1, "type": 4 },
        { "name": "BEAT_LOOP_8",   "bits": 1, "type": 4 },
        {                          "bits": 1, "type": 1 },
        { "name": "SLIP",          "bits": 1, "type": 4 },

        // byte 8
        { "name": "MEMORY³",       "bits": 1, "type": 4 },
        { "name": "MEM_DELETE³",   "bits": 1, "type": 4 },
        { "name": "MEM_CUE_FWD³",  "bits": 1, "type": 4 },
        { "name": "MEM_CUE_REV³",  "bits": 1, "type": 4 },
        {                          "bits": 1, "type": 1 },
        { "name": "CALL_DELETE",   "bits": 1, "type": 4 },
        {                          "bits": 2, "type": 1 },
    
        // byte 9
        { "name": "HOTCUE_A",       "bits": 1, "type": 4 },
        { "name": "HOTCUE_B",       "bits": 1, "type": 4 },
        { "name": "HOTCUE_C",       "bits": 1, "type": 4 },
        { "name": "HOTCUE_D",       "bits": 1, "type": 4 },
        { "name": "HOTCUE_E",       "bits": 1, "type": 4 },
        { "name": "HOTCUE_F",       "bits": 1, "type": 4 },
        { "name": "HOTCUE_G",       "bits": 1, "type": 4 },
        { "name": "HOTCUE_H",       "bits": 1, "type": 4 },

        // byte 10
        { "name": "SOURCE⁴",        "bits": 1, "type": 4 },
        { "name": "BROWSE⁴",        "bits": 1, "type": 4 },
        { "name": "TAGLIST⁴",       "bits": 1, "type": 4 },
        { "name": "PLAYLIST⁴",      "bits": 1, "type": 4 },
        { "name": "SEARCH⁴",        "bits": 1, "type": 4 },
        { "name": "MENU⁴",          "bits": 1, "type": 4 },
        {                           "bits": 1, "type": 1 },
        { "name": "JOG_MODE",       "bits": 1, "type": 4 },

        // byte 11
        { "name": "BACK⁵",          "bits": 1, "type": 4 },
        { "name": "TAG_TRACK⁵",     "bits": 1, "type": 4 },
        { "name": "TRACK_FILTER⁵",  "bits": 1, "type": 4 },
        { "name": "SHORTCUT⁵",      "bits": 1, "type": 4 },
        { "name": "ROTARY_SEL⁵",    "bits": 1, "type": 4 },
        {                           "bits": 1, "type": 1 },
        { "name": "TIME",           "bits": 1, "type": 4 },
        { "name": "QUANTIZE",       "bits": 1, "type": 4 },

        // byte 12
        { "name": "SD",             "bits": 1, "type": 4 },
        { "name": "USB_STOP",       "bits": 1, "type": 4 },
        {                           "bits": 6 },

        // byte 13
        {                           "bits": 8, "type": 1 },

        // byte 14-15
        { "name": "ROTARY_ENCODER_POS", "bits": 16 },

        // byte 16-17
        { "name": "TOUCHSCREEN_X",      "bits": 16 },

        // byte 18-19
        { "name": "TOUCHSCREEN_Y",      "bits": 16 },

        // byte 20-21
        { "name": "TBD?",               "bits": 16 },

        // byte 22-23
        { "name": "TEMPO_SLIDER_POS",   "bits": 16 },

        // byte 24
        { "name": "VINYL_SPEED_POS",    "bits": 8 },

        // byte 25-26
        { "name": "JOG_POS", "bits": 16 },

        // byte 27-28
        { "name": "JOG_SPEED", "bits": 16 },

        // byte 29
        { "name": "JOG_START?",    "bits": 1 },
        { "name": "JOG_PRESS",     "bits": 1 },
        { "name": "JOG_DIR",       "bits": 1 },
        { "name": "JOG_MOVING",    "bits": 1 },
        {                          "bits": 4, "type": 1 },
    ];

    const options = {
        fontsize: FONT_SIZE,
        label: {left: 0},
        margin: {left: LEFT_MARGIN, top: TOP_MARGIN},
        lanes: 30,
        bits: 240
    };

    const jsonml = render(reg, options);
    const svg = onml.stringify(jsonml);

    try {
    fs.writeFileSync('subucom-input.svg', svg);
    } catch (err) {
    console.error(err);
    }
}

function subucom_input_crc() {
    const reg =[
        // byte 62-63
        { "name": "CRC16", "bits": 16 },
    ];

    const options = {
        fontsize: FONT_SIZE,
        label: {left: 62},
        margin: {left: LEFT_MARGIN, top: TOP_MARGIN},
        lanes: 2,
        bits: 16
    };

    const jsonml = render(reg, options);
    const svg = onml.stringify(jsonml);

    try {
    fs.writeFileSync('subucom-input-crc.svg', svg);
    } catch (err) {
    console.error(err);
    }
}

function subucom_output() {
    const reg =[
        // byte 0
        {                          "bits": 8, "type": 1 },

        // byte 1
        {                          "bits": 8, "type": 1 },

        // byte 2
        { name: "KEYSYNC¹",        "bits": 2, "type": 7 },
        { name: "BEATSYNC¹",       "bits": 2 },
        { name: "MASTER¹",         "bits": 2, "type": 5 },
        {                          "bits": 2, "type": 1 },

        // byte 3
        { name: "JOG_RED¹",        "bits": 2, "type": 2 },
        { name: "JOG_WHITE¹",      "bits": 2 },
        { name: "SLIP¹",           "bits": 2, "type": 2 },
        { name: "QUANTIZE¹",       "bits": 2, "type": 2 },

        // byte 4
        { name: "SOURCE²",         "bits": 2 },
        { name: "BROWSE²",         "bits": 2, "type": 7 },
        { name: "TAGLIST²",        "bits": 2, "type": 7 },
        { name: "PLAYLIST²",       "bits": 2, "type": 7 },

        // byte 5
        { name: "SEARCH²",         "bits": 2, "type": 7 },
        { name: "MENU²",           "bits": 2 },
        {                          "bits": 4, "type": 1 },

        // byte 6
        {                          "bits": 8, "type": 1 },

        // byte 7
        { name: "PLAY",            "bits": 1, "type": 6 },
        { name: "CUE",             "bits": 1, "type": 5 },
        {                          "bits": 1, "type": 1 },
        { name: "CUE_IN",          "bits": 1, "type": 5 },
        { name: "CUE_OUT",         "bits": 1, "type": 5 },
        { name: "RELOOP",          "bits": 1, "type": 5 },
        { name: "BEAT_LOOP_4",     "bits": 1, "type": 5 },
        { name: "BEAT_LOOP_8",     "bits": 1, "type": 5 },

        // byte 8
        { name: "BEATJUMP_FWD",    "bits": 1 },
        { name: "BEATJUMP_REV",    "bits": 1 },
        {                          "bits": 1, "type": 1 },
        { name: "TEMPO_RESET",     "bits": 1, "type": 3 },
        { name: "MASTER_TEMPO",    "bits": 1, "type": 2 },
        {                          "bits": 1, "type": 1 },
        { name: "CDJ_MODE",        "bits": 1, "type": 3 },
        { name: "VINYL_MODE",      "bits": 1, "type": 7 },

        // byte 9
        { name: "ROTARY_ENC",      "bits": 1 },
        {                          "bits": 1, "type": 1 },
        { name: "SEARCH_MEM³",     "bits": 1, "type": 5 },
        {                          "bits": 1, "type": 1 },
        { name: "REV",             "bits": 1, "type": 2 },
        {                          "bits": 2, "type": 1 },
        { name: "EUP",             "bits": 1, "type": 2 },

        // byte 10
        {                          "bits": 8, "type": 1 },

        // byte 11
        {                          "bits": 8, "type": 1 },

        // byte 12
        { name: "HOTCUE_A (R)⁴",    "bits": 8, "type": 2 },

        // byte 13
        { name: "HOTCUE_A (G)⁴",    "bits": 8, "type": 3 },

        // byte 14
        { name: "HOTCUE_A (B)⁴",    "bits": 8, "type": 7 },

        // byte 15
        { name: "HOTCUE_B (R)⁴",    "bits": 8, "type": 2 },

        // byte 16
        { name: "HOTCUE_B (G)⁴",    "bits": 8, "type": 3 },

        // byte 17
        { name: "HOTCUE_B (B)⁴",    "bits": 8, "type": 7 },

        // byte 18
        { name: "HOTCUE_C (R)⁴",    "bits": 8, "type": 2 },

        // byte 19
        { name: "HOTCUE_C (G)⁴",    "bits": 8, "type": 3 },

        // byte 20
        { name: "HOTCUE_C (B)⁴",    "bits": 8, "type": 7 },

        // byte 21
        { name: "HOTCUE_D (R)⁴",    "bits": 8, "type": 2 },

        // byte 22
        { name: "HOTCUE_D (G)⁴",    "bits": 8, "type": 3 },

        // byte 23
        { name: "HOTCUE_D (B)⁴",    "bits": 8, "type": 7 },

        // byte 24
        { name: "HOTCUE_E (R)⁴",    "bits": 8, "type": 2 },

        // byte 25
        { name: "HOTCUE_E (G)⁴",    "bits": 8, "type": 3 },

        // byte 26
        { name: "HOTCUE_E (B)⁴",    "bits": 8, "type": 7 },

        // byte 27
        { name: "HOTCUE_F (R)⁴",    "bits": 8, "type": 2 },

        // byte 28
        { name: "HOTCUE_F (G)⁴",    "bits": 8, "type": 3 },

        // byte 29
        { name: "HOTCUE_F (B)⁴",    "bits": 8, "type": 7 },

        // byte 30
        { name: "HOTCUE_G (R)⁴",    "bits": 8, "type": 2 },

        // byte 31
        { name: "HOTCUE_G (G)⁴",    "bits": 8, "type": 3 },

        // byte 32
        { name: "HOTCUE_G (B)⁴",    "bits": 8, "type": 7 },

        // byte 33
        { name: "HOTCUE_H (R)⁴",    "bits": 8, "type": 2 },

        // byte 34
        { name: "HOTCUE_H (G)⁴",    "bits": 8, "type": 3 },

        // byte 35
        { name: "HOTCUE_H (B)⁴",    "bits": 8, "type": 7 },

        // byte 36
        { name: "SD (R)⁴",          "bits": 8, "type": 2 },

        // byte 37
        { name: "SD (G)⁴",          "bits": 8, "type": 3 },

        // byte 38
        { name: "SD (B)⁴",          "bits": 8, "type": 7 },

        // byte 39
        { name: "USB (R)⁴",         "bits": 8, "type": 2 },

        // byte 40
        { name: "USB (G)⁴",         "bits": 8, "type": 3 },

        // byte 41
        { name: "USB (B)⁴",         "bits": 8, "type": 7 },

        // byte 42
        { name: "MEDIA_COLOR (R)⁴", "bits": 8, "type": 2 },

        // byte 43
        { name: "MEDIA_COLOR (G)⁴", "bits": 8, "type": 3 },

        // byte 44
        { name: "MEDIA_COLOR (B)⁴", "bits": 8, "type": 7 },
    ];

    const options = {
        fontsize: FONT_SIZE,
        label: {left: 0},
        margin: {left: LEFT_MARGIN, top: TOP_MARGIN},
        lanes: 45,
        bits: 360
    };

    const jsonml = render(reg, options);
    const svg = onml.stringify(jsonml);

    try {
    fs.writeFileSync('subucom-output.svg', svg);
    } catch (err) {
    console.error(err);
    }
}

function subucom_output_crc() {
    const reg =[
        // byte 62-63
        { "name": "CRC16", "bits": 16 },
    ];

    const options = {
        fontsize: FONT_SIZE,
        label: {left: 62},
        margin: {left: LEFT_MARGIN, top: TOP_MARGIN},
        lanes: 2,
        bits: 16
    };

    const jsonml = render(reg, options);
    const svg = onml.stringify(jsonml);

    try {
    fs.writeFileSync('subucom-output-crc.svg', svg);
    } catch (err) {
    console.error(err);
    }
}

subucom_input();
subucom_input_crc();
subucom_output();
subucom_output_crc();
