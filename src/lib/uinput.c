// SPDX-License-Identifier: GPL-2.0
/*
 *  CDJ3K uinput driver
 * 
 *  This file is part of the Magic Phono project (https://magicphono.org/).
 *  Copyright (c) 2025 xorbxbx <xorbxbx@magicphono.org>
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include "uinput.h"
#include "keymap.h"

#define REPEAT_MS      30

const char *uinput_device_path = "/dev/uinput";

static int64_t current_time_millis() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int64_t s1 = (int64_t)(time.tv_sec) * 1000;
  int64_t s2 = time.tv_usec / 1000;
  return s1 + s2;
}

static void emit(int fd, int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fd, &ie, sizeof(ie));
}

void uinput_emit(uinput_t* uinput, int type, int code, int val)
{
   /* ignore reserved code */
   if (type == EV_KEY && code == 0) {
      return;
   }

   /* rate limit repeats */
   static int64_t last_tv_msec = 0;
   int64_t tv_msec = current_time_millis();
   if (val == 2) {
      if (tv_msec < last_tv_msec + REPEAT_MS) {
         return;
      }
      last_tv_msec = tv_msec;
   }

   // printf("uinput: emitting: fd=%d, type=%02x, code=%d, val=%d\n", uinput->fd, type, code, val);

   emit(uinput->fd, type, code, val);
   emit(uinput->fd, EV_SYN, SYN_REPORT, 0);
}

int uinput_init(uinput_t* uinput, keymap_t* keymap)
{
   struct uinput_setup usetup;

   int fd = open(uinput_device_path, O_WRONLY | O_NONBLOCK);
   if (fd < 0) {
      fprintf(stderr, "Error opening device %s: %s\n", uinput_device_path, strerror(errno));
      return -1;
   }

   uinput->fd = fd;

   ioctl(fd, UI_SET_EVBIT, EV_KEY);
   keymap_register_uinput_keycodes(keymap, fd);

   memset(&usetup, 0, sizeof(usetup));
   usetup.id.bustype = BUS_SPI;
   usetup.id.vendor = 0x1234; /* sample vendor */
   usetup.id.product = 0x5678; /* sample product */
   strcpy(usetup.name, "Subucom Input Device");

   ioctl(fd, UI_DEV_SETUP, &usetup);
   ioctl(fd, UI_DEV_CREATE);

   /*
    * On UI_DEV_CREATE the kernel will create the device node for this
    * device. We are inserting a pause here so that userspace has time
    * to detect, initialize the new device, and can start listening to
    * the event, otherwise it will not notice the event we are about
    * to send.
    */
   sleep(1);

   return 0;
}

void uinput_deinit(uinput_t* uinput) {
   /*
    * Give userspace some time to read the events before we destroy the
    * device with UI_DEV_DESTOY.
    */
   sleep(1);

   ioctl(uinput->fd, UI_DEV_DESTROY);
   close(uinput->fd); 
}
