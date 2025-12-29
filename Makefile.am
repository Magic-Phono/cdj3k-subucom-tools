AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = subucom_blink subucom_uinput subucom_reset_timer subucom_check subucom_dump

subucom_blink_SOURCES = src/subucom_blink.c \
  src/lib/crc16.c \
  src/lib/subucom.c

subucom_check_SOURCES = src/subucom_check.c \
  src/lib/crc16.c \
  src/lib/subucom.c

subucom_dump_SOURCES = src/subucom_dump.c \
  src/lib/crc16.c \
  src/lib/subucom.c

subucom_reset_timer_SOURCES = src/subucom_reset_timer.c \
  src/lib/crc16.c \
  src/lib/subucom.c

subucom_uinput_SOURCES = src/subucom_uinput.c \
  src/lib/crc16.c \
  src/lib/doom_keymap.c \
  src/lib/uinput.c \
  src/lib/subucom.c

subucom_dump_CFLAGS = -lncurses -ltinfo
