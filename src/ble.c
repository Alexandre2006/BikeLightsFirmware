#include "ble.h"
#include "dfu.h"

#include <sys/_intsup.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_backend_ble.h>

// Custom Services
BT_GATT_SERVICE_DEFINE(dfu_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_DFU_SERVICE),
 BT_GATT_CHARACTERISTIC(BT_UUID_DFU_CHAR, BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_WRITE, NULL, write_dfu, NULL));

BT_GATT_SERVICE_DEFINE(config_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_CONFIG_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_CONFIG_MODE_CHAR, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CONFIG_BLINKING_SPEED_CHAR, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CONFIG_HEADLIGHT_BRIGTHNESS_CHAR, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CONFIG_BRAKE_BRIGTHNESS_CHAR, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CONFIG_BLINKER_BRIGTHNESS_CHAR, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CONFIG_BLINKER_MODE_CHAR, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE, NULL, NULL, NULL));

// Register log module
LOG_MODULE_REGISTER(ble_log);

// Bluetooth advertising data
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, LOGGER_BACKEND_BLE_ADV_UUID_DATA),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

// Advertising
static int start_advertising(void) {
    int err;

    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return err;
    }

    LOG_INF("Advertising successfully started");
    return 0;
}

// Bluetooth Connection Callbacks
static void connected(struct bt_conn *conn, uint8_t err) {
    if (err) {
        LOG_ERR("Connection failed (err %u %s)", err, bt_hci_err_to_str(err));
    } else {
        LOG_INF("Connected");
    }
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
    LOG_INF("Disconnected (reason %u %s)", reason, bt_hci_err_to_str(reason));
    start_advertising();
}

struct bt_conn_cb cb_callbacks = {
.connected = connected,
.disconnected = disconnected,
};

// Logging Backend Hook
static void backend_ble_hook(bool status, void *ctx) {
    ARG_UNUSED(ctx);

    // Log status
    if (status) {
        LOG_INF("BLE Logging Backend Enabled");
    } else {
        LOG_INF("BLE Logging Backend Disabled");
    }
}

// Bluetooth Initialization
int start_ble(void) {
    int err;

    // Configure logging backend
    logger_backend_ble_set_hook(backend_ble_hook, NULL);

    // Enable bluetooth
    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth initialization failed (err %d)", err);
        return err;
    }

    // Register connection callbacks
    err = bt_conn_cb_register(&cb_callbacks);
    if (err) {
        LOG_ERR("Connection callbacks registration failed (err %d)", err);
        return err;
    }

    // Start advertising
    err = start_advertising();
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return err;
    }

    return 0;
}


