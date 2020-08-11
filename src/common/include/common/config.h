/*
 * Copyright (C) 2020 Evan Lojewski
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _COMMON_CONFIG_H
#define _COMMON_CONFIG_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

struct config_t;
typedef struct config_t config_t;

typedef void (*config_callback_t)(struct config_t *entry);

typedef struct {
    int32_t max_value;
    int32_t min_value;
    int32_t value;
} config_value_t;

struct config_t {
    const unsigned char config_id[4]; /* Note: SDCC does not support uint32_t being set to values such as 'ASCI', as such, this an explicit array instead */
    const char *config_short;
    const char *config_desc;
    config_callback_t set_callback;
    config_value_t value;
    struct config_t* next;
} __xdata;

/**
 * Register a configuration type
 */
bool config_register(config_t *entry) __reentrant;

/**
 * Get the configuration handler for a given name.
 */
config_t *config_get_config(const char *config_short);

/**
 * Get the configuration handler by index.
 */
config_t *config_index(int32_t index);

/**
 * Get the next registered configuration item.
 */
config_t *config_next(config_t *current);

/**
 * Update the configuration value, calling callbacks as needed.
 */
bool config_set_value(config_t *config, int32_t value) __reentrant;

/**
 * Retrieve the current configuration value
 */
int32_t config_get_value(config_t *config) __reentrant;

/**
 * Rewrite all configuration values into flash.
 *
 * Note: This will compact flash by erasing and then writing
 * all known configuration values.
 */
bool config_save_entries() __reentrant;

#endif // _COMMON_CONFIG_H
