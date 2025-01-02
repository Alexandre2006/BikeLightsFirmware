#pragma once

#include <zephyr/bluetooth/att.h>
#include <zephyr/bluetooth/gatt.h>
#include <sys/types.h>

enum GlobalMode {
    GLOBAL_OFF,
    GLOBAL_REAR_ONLY,
    GLOBAL_ON,
    GLOBAL_HAZAR
};

enum BlinkerMode {
    BLINKER_OFF,
    BLINKER_LEFT,
    BLINKER_RIGHT
};

int initialize_settings(void);

ssize_t get_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);

ssize_t set_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);