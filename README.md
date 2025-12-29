# cdj3k-subucom-tools

This package contains the CDJ3K Subucom Utilities, version @VERSION@.

Copyright (c) 2025 Magic Phono Development Team <xorbxbx@magicphono.org>

All files in this package can be freely distributed and used according
to the terms of the GNU General Public License, either version 2 or
(at your opinion) any newer version. See https://www.gnu.org/ for details.

## 1. What's that?

The CDJ3K Subucom Utilities package contains tools to interact with the
subucom controller from the main SoC of the AlphaTheta CDJ-3000 player.

The utilities include:

  - subucom_blink: blinks the LEDs continously, useful for testing.

  - subucom_check: checks for magic key press combo and writes output to file.

  - subucom_dump: reads subucom controller state and dumps to the screen
    continously.

  - subucom_uinput: userspace application that reads subucom controller
    state and emits events to an uinput virtual input device.

## 2. What's subucom?

Subucom (aka SUB MICROCOMputer) is a dedicated microcontroller in the CDJ that
manages the buttons and controls on the player, including the jog wheel,
touchscreen and LEDs. It is connected to the Main SoC via an SPI interface.

For protocol information, see docs.

## 3. Building

Install the cdj3k-toolchain and make sure the poky/oe environment is
initialized before building.

To build, run:

  ./do_build.sh

The repo can be cleaned by running:

  make distclean
