// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K subucom interface
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#ifndef __SUBUCOM_H_
#define __SUBUCOM_H_

#include <linux/types.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include "keymap.h"

#define SUBUCOM_BUFSIZE      64

#ifdef DEBUG_SUBUCOM
#define PRINT(...) printf( __VA_ARGS__ )
#else
#define PRINT(...) do{ } while ( 0 )
#endif

enum read_mode {
	REGULAR,
	POLLED
};

typedef void (*input_event_cb_t)(int type, int code, int val);

typedef struct subucom {
	int			 	 fd;
    struct pollfd    fds[1];
    input_event_cb_t fire_input_event_fn;

	enum read_mode   _read_mode;
	uint8_t*         _buf;
    uint8_t*         _prev_buf;
    keymap_t*        _keymap;
} subucom_t;

/* Read / Write timer status */
#define SUBUCOM_IOC_MAGIC		'p'
#define SUBUCOM_IOC_RD_TIMER_STATUS	_IOR(SUBUCOM_IOC_MAGIC, 1, __u8)
#define SUBUCOM_IOC_WR_TIMER_STATUS	_IOW(SUBUCOM_IOC_MAGIC, 1, __u8)

/* Read / Write timer interval msec */
#define SUBUCOM_IOC_RD_TIMER_INTERVAL	_IOR(SUBUCOM_IOC_MAGIC, 2, __u32)
#define SUBUCOM_IOC_WR_TIMER_INTERVAL	_IOW(SUBUCOM_IOC_MAGIC, 2, __u32)

int  subucom_init(subucom_t* subucom, const char *device_path);
int  subucom_register_keymap(subucom_t* subucom, keymap_t* keymap, input_event_cb_t fire_input_event_cb);
void subucom_deinit(const subucom_t* subucom);

/* low level functions */
int  subucom_read(subucom_t* subucom);
int  subucom_write(subucom_t* subucom, const uint8_t* buf, const uint8_t len);

void subucom_start_timer(subucom_t* subucom, int tick_ms);
void subucom_stop_timer(subucom_t* subucom);
int  subucom_is_timer_running(subucom_t* subucom);
int  subucom_read_timer_interval(subucom_t* subucom);

#endif /* __SUBUCOM_H_ */
