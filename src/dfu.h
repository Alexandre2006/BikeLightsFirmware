#pragma once

#include <sys/types.h>
#include <zephyr/bluetooth/gatt.h>

ssize_t write_dfu(struct bt_conn *conn,
			 const struct bt_gatt_attr *attr,
			 const void *buf,
			 uint16_t len, uint16_t offset, uint8_t flags);