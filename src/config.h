#pragma once

#include <zephyr/bluetooth/att.h>
#include <zephyr/bluetooth/gatt.h>
#include <sys/types.h>

enum GlobalMode {
    GLOBAL_OFF,
    GLOBAL_REAR_ONLY,
    GLOBAL_ON,
};

enum BlinkerMode {
    BLINKER_OFF,
    BLINKER_LEFT,
    BLINKER_RIGHT,
    BLINKER_HAZARD,
};

enum BrakeMode {
    BRAKE_IDLE,
    BRAKE_BRAKING,
};

struct light_config {
    enum GlobalMode mode;
    int blinking_speed;
    int headlight_brightness;
    int brake_idle_brightness;
    int brake_braking_brightness;
    int blinker_brightness;
    enum BrakeMode brake_mode;
    enum BlinkerMode blinker_mode;
};

extern struct k_msgq config_msgq;

int initialize_settings(void);

ssize_t get_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);
ssize_t get_blinking_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);
ssize_t get_headlight_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);
ssize_t get_brake_idle_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);
ssize_t get_brake_braking_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);
ssize_t get_blinker_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);
ssize_t get_brake_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);
ssize_t get_blinker_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset);

ssize_t set_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);
ssize_t set_blinking_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);
ssize_t set_headlight_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);
ssize_t set_brake_idle_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);
ssize_t set_brake_braking_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);
ssize_t set_blinker_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);
ssize_t set_brake_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);
ssize_t set_blinker_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags);