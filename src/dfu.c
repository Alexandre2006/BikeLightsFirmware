#include "dfu.h"

#include <zephyr/logging/log.h>

#define DFU_OTA_ONLY 0xA8    // Enter OTA DFU mode (BLE)
#define DFU_SERIAL_ONLY 0x4E // Enter serial only mode
#define DFU_UF2 0x57    // Enter UF2 mode (CDC + MSC -- both UF2 and serial)
#define DFU_SKIP 0x6D   // Skip DFU, boot normally
#define DFU_NORMAL 0x00 // Boot normally

#define BLE_COMMAND_DFU_OTA_ONLY 0x01
#define BLE_COMMAND_DFU_SERIAL_ONLY 0x02
#define BLE_COMMAND_DFU_UF2 0x03
#define BLE_COMMAND_DFU_SKIP 0x04
#define BLE_COMMAND_DFU_NORMAL 0x00

LOG_MODULE_REGISTER(dfu);

ssize_t write_dfu(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         const void *buf, uint16_t len, uint16_t offset,
                         uint8_t flags) {

  // Check for invalid command (length/offest)
  if (len != 1U) {
    LOG_ERR("DFU: Incorrect length: %u", len);
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  if (offset != 0) {
    LOG_ERR("DFU: Invalid offset: %u", offset);
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  // Read received command
  uint8_t *value = (uint8_t *)buf;

  // Process value
  switch (*value) {
  case BLE_COMMAND_DFU_OTA_ONLY:
    LOG_INF("DFU: OTA only mode");
    NRF_POWER->GPREGRET = DFU_OTA_ONLY;
    break;
  case BLE_COMMAND_DFU_SERIAL_ONLY:
    LOG_INF("DFU: Serial only mode");
    NRF_POWER->GPREGRET = DFU_SERIAL_ONLY;
    break;
  case BLE_COMMAND_DFU_UF2:
    LOG_INF("DFU: UF2 mode");
    NRF_POWER->GPREGRET = DFU_UF2;
    break;
  case BLE_COMMAND_DFU_SKIP:
    LOG_INF("DFU: Skip mode");
    NRF_POWER->GPREGRET = DFU_SKIP;
    break;
  case BLE_COMMAND_DFU_NORMAL:
    LOG_INF("DFU: Normal mode");
    NRF_POWER->GPREGRET = DFU_NORMAL;
    break;
  default:
    LOG_ERR("DFU: Invalid command: %u", *value);
    return BT_GATT_ERR(BT_ATT_ERR_NOT_SUPPORTED);
  }

  // Reset device
  NVIC_SystemReset();
}