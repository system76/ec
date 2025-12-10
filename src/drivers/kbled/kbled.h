// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#ifndef _DRIVERS_KBLED_KBLED_H
#define _DRIVERS_KBLED_KBLED_H

#include <stdint.h>

#if CONFIG_HAVE_KBLED

enum KbledKind {
    KBLED_NONE = 0,
    KBLED_WHITE = 1,
    KBLED_RGB = 2,
};
extern enum KbledKind kbled_kind;

// Must be specified per mechanism
void kbled_init(void);
void kbled_reset(void);
uint8_t kbled_get(void);
uint8_t kbled_max(void);
void kbled_set(uint8_t level);
uint32_t kbled_get_color(void);
void kbled_set_color(uint32_t color);

// Provided by common code
void kbled_hotkey_color(void);
void kbled_hotkey_down(void);
void kbled_hotkey_up(void);
void kbled_hotkey_toggle(void);
void kbled_hotkey_step(void);

#else // CONFIG_HAVE_KBLED

static inline void kbled_init(void) {}
static inline void kbled_reset(void) {}

static inline uint8_t kbled_get(void) {
    return 0;
}

static inline uint8_t kbled_max(void) {
    return 0;
}

static inline void kbled_set(uint8_t level) {
    (void)level;
}

static inline uint32_t kbled_get_color(void) {
    return 0;
}

static inline void kbled_set_color(uint32_t color) {
    (void)color;
}

static inline void kbled_hotkey_color(void) {}
static inline void kbled_hotkey_down(void) {}
static inline void kbled_hotkey_up(void) {}
static inline void kbled_hotkey_toggle(void) {}
static inline void kbled_hotkey_step(void) {}

#endif // CONFIG_HAVE_KBLED

#endif // _DRIVERS_KBLED_KBLED_H
