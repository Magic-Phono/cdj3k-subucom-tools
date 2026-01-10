// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K subucom interface
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/ioctl.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include "subucom.h"
#include "crc16.h"
#include "uinput.h"


const char *default_subucom_device_path = "/dev/subucom_spi2.0";

int subucom_init(subucom_t* subucom, const char *device_path) {
    const char *subucom_device_path = default_subucom_device_path;
    if (device_path != NULL) {
        subucom_device_path = device_path;
    }

    int fd = open(subucom_device_path, O_RDWR);

    if (fd < 0) {
        fprintf(stderr, "Error opening device %s: %s\n", subucom_device_path, strerror(errno));
        return -1;
    }

    subucom->_read_mode = REGULAR;
    subucom->fd = fd;
    subucom->fire_input_event_fn = NULL;

    subucom->fds[0].fd = fd;
    subucom->fds[0].events = POLLIN;

    uint8_t* ptr = (uint8_t *)malloc(SUBUCOM_BUFSIZE);
    if (ptr == NULL) {
        return -1;
    }
    subucom->_buf = ptr;

    uint8_t* prev_ptr = (uint8_t *)malloc(SUBUCOM_BUFSIZE);
    if (prev_ptr == NULL) {
        return -1;
    }
    subucom->_prev_buf = prev_ptr;

    return 0;
}

int subucom_register_keymap(subucom_t* subucom, keymap_t* keymap, input_event_cb_t fire_input_event_cb) {
    subucom->_keymap = keymap;
    subucom->fire_input_event_fn = fire_input_event_cb;

    return 0;
} 

void subucom_stop_timer(subucom_t* subucom) {
    int val = 0;
    ioctl(subucom->fd, SUBUCOM_IOC_WR_TIMER_STATUS, &val);

    subucom->_read_mode = REGULAR;
}

void subucom_start_timer(subucom_t* subucom, int tick_ms) {
    if (subucom_is_timer_running(subucom)) {
        subucom_stop_timer(subucom);
    }

    int val = tick_ms;
    ioctl(subucom->fd, SUBUCOM_IOC_WR_TIMER_INTERVAL, &val);

    val = 1;
    ioctl(subucom->fd, SUBUCOM_IOC_WR_TIMER_STATUS, &val);

    subucom->_read_mode = POLLED;
}

int subucom_is_timer_running(subucom_t* subucom)
{
    int val;
    ioctl(subucom->fd, SUBUCOM_IOC_RD_TIMER_STATUS, &val);
    return val;
}

int subucom_read_timer_interval(subucom_t* subucom)
{
    int val;
    ioctl(subucom->fd, SUBUCOM_IOC_RD_TIMER_INTERVAL, &val);
    return val;    
}

static uint16_t be16_to_cpu_unsigned(const uint8_t data0, const uint8_t data1)
{
    return ((uint16_t)data0 << 8) | (uint16_t)data1;
}

static int validate_checksum(uint8_t* buf) {
    uint16_t crc16 = crc16_x25_calc(buf, SUBUCOM_BUFSIZE-2);
    uint8_t crch = (crc16 & 0xFF);
    uint8_t crcl = (crc16 >> 8);
    if (crch != buf[SUBUCOM_BUFSIZE-2] || crcl != buf[SUBUCOM_BUFSIZE-1]) {
        return -1;
    }

    return 0;
}

inline static void fire_input_event(subucom_t* subucom, int type, int code, int val)
{
    if (subucom->fire_input_event_fn != NULL) {
        subucom->fire_input_event_fn(type, code, val);
    }
}

static void read_jog(subucom_t* subucom, const uint8_t *buffer, const uint8_t *prev_buffer)
{
    const uint8_t MOVING = (1 << 3);
    const uint8_t DIR = (1 << 2);
    const uint8_t PRESS = (1 << 1);

    uint8_t num_jogs = subucom->_keymap->num_jogs;
    for (int i=0; i<num_jogs; i++) {
        jog_def_t jog = subucom->_keymap->jogs[i];

        uint8_t moving = buffer[jog.byte] & MOVING;
        uint8_t dir = buffer[jog.byte] & DIR;
        uint8_t pressed = buffer[jog.byte] & PRESS;

        uint8_t moving_prev = prev_buffer[jog.byte] & MOVING;
        uint8_t dir_prev = prev_buffer[jog.byte] & DIR;
        uint8_t pressed_prev = prev_buffer[jog.byte] & PRESS;
        
        if (jog.dir_as_button == true) {
            if (moving != moving_prev) {
                if (moving == MOVING) {
                    PRINT("jog %d pressed\n", dir);
                    if (dir == 0) {
                        fire_input_event(subucom, EV_KEY, jog.left_keycode, 1);
                    } else {
                        fire_input_event(subucom, EV_KEY, jog.right_keycode, 1);
                    }
                } else {
                    PRINT("jog %d released\n", dir);
                    fire_input_event(subucom, EV_KEY, jog.left_keycode, 0);
                    fire_input_event(subucom, EV_KEY, jog.right_keycode, 0);
                }
            } else if (moving == MOVING) {
                if (dir == dir_prev) {
                    PRINT("jog %d repeat\n", dir);
                    fire_input_event(subucom, EV_KEY, (dir == 0 ? jog.left_keycode : jog.right_keycode), 2);
                } else {
                    if (dir == 0) {
                        PRINT("jog %d dir changed\n", dir);
                        fire_input_event(subucom, EV_KEY, jog.right_keycode, 0);
                        fire_input_event(subucom, EV_KEY, jog.left_keycode, 1);
                    } else {
                        PRINT("jog %d dir changed\n", dir);
                        fire_input_event(subucom, EV_KEY, jog.left_keycode, 0);
                        fire_input_event(subucom, EV_KEY, jog.right_keycode, 1);
                    }
                }
            }
        }

        if (pressed != pressed_prev) {
            if (pressed == PRESS) {
                PRINT("jog button pressed\n");
                fire_input_event(subucom, EV_KEY, jog.button_keycode, 1);
            } else {
                PRINT("jog button released\n");
                fire_input_event(subucom, EV_KEY, jog.button_keycode, 0);
            }
        } else if (pressed == PRESS) {
            PRINT("jog button repeat\n");
                fire_input_event(subucom, EV_KEY, jog.button_keycode, 2);
        }
    }
}

static void read_selectors(subucom_t* subucom, const uint8_t *buffer, const uint8_t *prev_buffer) {
    uint8_t num_selectors = subucom->_keymap->num_selectors;
    for (int i=0; i<num_selectors; i++) {
        selector_def_t selector = subucom->_keymap->selectors[i];
        selector_state_t *states = selector.states;

        uint8_t selector_state = buffer[selector.byte];
        uint8_t selector_state_prev = prev_buffer[selector.byte];

        for (int j; j<selector.state_count; j++) {
            selector_state_t state = states[j];
            if (state.as_button == true) {
                if (selector_state != selector_state_prev) {
                    if (selector_state == state.value) {
                        PRINT("selector %d:%d pressed\n", selector.id, state.id);
                        fire_input_event(subucom, EV_KEY, state.keycode, 1);
                    } else {
                        PRINT("selector %d:%d released\n", selector.id, state.id);
                        fire_input_event(subucom, EV_KEY, state.keycode, 0);
                    }
                } else {
                    if (selector_state == state.value) {
                        PRINT("selector %d:%d repeat\n", selector.id, state.id);
                        fire_input_event(subucom, EV_KEY, state.keycode, 2);
                    }
                }
            }
        }
    }
}

static void read_buttons(subucom_t* subucom, const uint8_t *buffer, const uint8_t *prev_buffer) {
    uint8_t num_buttons = subucom->_keymap->num_buttons;
    for (int i=0; i<num_buttons; i++) {
        button_def_t button = subucom->_keymap->buttons[i];

        uint8_t button_state = buffer[button.byte] & button.bit;
        uint8_t button_state_prev = prev_buffer[button.byte] & button.bit;

        bool button_pressed = (button_state == button.bit);

        if (button_state != button_state_prev) {
            if (button_state == button.bit) {
                PRINT("button %d pressed\n", button.id);
                fire_input_event(subucom, EV_KEY, button.keycode, 1);
            } else {
                PRINT("button %d released\n", button.id);
                fire_input_event(subucom, EV_KEY, button.keycode, 0);
            }
        } else if (button_pressed == true) {
            PRINT("button %d repeat\n", button.id);
            fire_input_event(subucom, EV_KEY, button.keycode, 2);
        }

    }
}

static void read_encoders(subucom_t* subucom, const uint8_t *buffer, const uint8_t *prev_buffer) {
    uint8_t num_encoders = subucom->_keymap->num_encoders;
    for (int i=0; i<num_encoders; i++) {
        encoder_def_t encoder = subucom->_keymap->encoders[i];

        int16_t encoder_value = be16_to_cpu_unsigned(buffer[encoder.byte], buffer[encoder.byte + 1]);
        int16_t encoder_value_prev = be16_to_cpu_unsigned(prev_buffer[encoder.byte], prev_buffer[encoder.byte + 1]);

        if (encoder.dir_as_button == true) {
            if (encoder_value > encoder_value_prev) {   
                PRINT("encoder %d pressed\n", encoder.right_id);
                PRINT("encoder %d released\n", encoder.right_id);
                fire_input_event(subucom, EV_KEY, encoder.right_keycode, 1);
                fire_input_event(subucom, EV_KEY, encoder.right_keycode, 0);
            } else if (encoder_value < encoder_value_prev) {
                PRINT("encoder %d pressed\n", encoder.left_id);
                PRINT("encoder %d released\n", encoder.left_id);
                fire_input_event(subucom, EV_KEY, encoder.left_keycode, 1);
                fire_input_event(subucom, EV_KEY, encoder.left_keycode, 0);
            }
        }
    }
}

int subucom_read(subucom_t* subucom) {
    static bool first_access = true;
    ssize_t bytes_read = 0;
    uint8_t* buf = subucom->_buf;
    int ret;

    if (subucom->_read_mode == POLLED) {
        int ret = poll(subucom->fds, 1, 5000); 

        if (ret > 0 && subucom->fds[0].revents & POLLIN) {
            bytes_read = read(subucom->fd, buf, SUBUCOM_BUFSIZE);

            if (bytes_read != SUBUCOM_BUFSIZE) {
                perror("Error reading from device");
                close(subucom->fd);
                return -1;
            }
        }
    } else {
        bytes_read = read(subucom->fd, buf, SUBUCOM_BUFSIZE);

        if (bytes_read != SUBUCOM_BUFSIZE) {
            perror("Error reading from device");
            close(subucom->fd);
            return -1;
        }
    }

    // first read, copy to previous buffer
    if (first_access == true) {
        memcpy(subucom->_prev_buf, buf, SUBUCOM_BUFSIZE);
        first_access = false;
    }

    #ifdef DEBUG_SUBUCOM_READ
    PRINT("Read %zd bytes from %s\n", bytes_read, subucom_device_path);
    for (size_t i = 0; i < (size_t)bytes_read; i++) {
        PRINT("%02x ", (unsigned char)buf[i]);
    }
    PRINT("\n");
    #endif

    // emit input events (if keymap is supplied)
    if (subucom->_keymap != NULL) {
        read_buttons(subucom, buf, subucom->_prev_buf);
        read_jog(subucom, buf, subucom->_prev_buf);
        read_encoders(subucom, buf, subucom->_prev_buf);
        read_selectors(subucom, buf, subucom->_prev_buf);
    }

    memcpy(subucom->_prev_buf, buf, SUBUCOM_BUFSIZE);

    // validate checksum
    ret = validate_checksum(buf);
    if (ret < 0) {
        fprintf(stderr, "subucom_read: Checksum failed\n");
        return -1;
    }

    return bytes_read;
}

int subucom_write(subucom_t* subucom, const uint8_t* buf, const uint8_t len) {
    if (subucom->_read_mode == POLLED) {
        fprintf(stderr, "subucom_write: Unavailable in POLLED mode\n");
        return -1;
    }

    if (len > SUBUCOM_BUFSIZE-2) {
        return -1;
    }

    char buffer[SUBUCOM_BUFSIZE];
    memcpy(buffer, buf, len);
    memset(buffer+len, 0x0, SUBUCOM_BUFSIZE-len);

    uint16_t crc16 = crc16_x25_calc(buffer, SUBUCOM_BUFSIZE-2);
    uint8_t crch = (crc16 & 0xFF);
    uint8_t crcl = (crc16 >> 8);

    buffer[SUBUCOM_BUFSIZE-2] = crch;
    buffer[SUBUCOM_BUFSIZE-1] = crcl;

    int ret = write(subucom->fd, buffer, SUBUCOM_BUFSIZE);

    if (ret < 0) {
        fprintf(stderr, "subucom_write: Error writing: %d %s\n", errno, strerror(errno));
        return -1;
    }

    return 0;
}

void subucom_deinit(const subucom_t* subucom) {
    close(subucom->fd);
    free(subucom->_buf);
    free(subucom->_prev_buf);
}
