#include "config.h"

#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

LOG_MODULE_REGISTER(config);

struct config {
  enum GlobalMode mode;
  int blinking_speed;
  int headlight_brightness;
  int brake_brightness;
  int blinker_brightness;
  enum BlinkerMode blinker_mode;
} light_config = {.mode = GLOBAL_OFF,
                  .blinking_speed = 60,
                  .headlight_brightness = 100,
                  .brake_brightness = 100,
                  .blinker_brightness = 100,
                  .blinker_mode = BLINKER_OFF};

// Handlers for Settings API
static int handle_get(const char *name, char *val, int val_len) {

  // Switch on the name of the setting
  if (!strcmp(name, "mode")) {
    if (val_len != sizeof(enum GlobalMode)) {
      return -EINVAL;
    }
    memcpy(val, &light_config.mode, sizeof(enum GlobalMode));
    return sizeof(enum GlobalMode);
  } else if (!strcmp(name, "blinking_speed")) {
    if (val_len != sizeof(int)) {
      return -EINVAL;
    }

    memcpy(val, &((struct config *)val)->blinking_speed, sizeof(int));
    return sizeof(int);
  } else if (!strcmp(name, "headlight_brightness")) {
    if (val_len != sizeof(int)) {
      return -EINVAL;
    }

    memcpy(val, &((struct config *)val)->headlight_brightness, sizeof(int));
    return sizeof(int);
  } else if (!strcmp(name, "brake_brightness")) {
    if (val_len != sizeof(int)) {
      return -EINVAL;
    }

    memcpy(val, &((struct config *)val)->brake_brightness, sizeof(int));
    return sizeof(int);
  } else if (!strcmp(name, "blinker_brightness")) {
    if (val_len != sizeof(int)) {
      return -EINVAL;
    }

    memcpy(val, &((struct config *)val)->blinker_brightness, sizeof(int));
    return sizeof(int);
  } else if (!strcmp(name, "blinker_mode")) {
    if (val_len != sizeof(enum BlinkerMode)) {
      return -EINVAL;
    }

    memcpy(val, &((struct config *)val)->blinker_mode,
           sizeof(enum BlinkerMode));
    return sizeof(enum BlinkerMode);
  }

  return -ENOENT;
}

static int handle_set(const char *name, size_t len, settings_read_cb read_cb,
                      void *cb_arg) {
  int err;

  if (strcmp(name, "mode") == 0) {
    err = read_cb(cb_arg, &light_config.mode, sizeof(light_config.mode));
    if (err < 0) {
      return err;
    }

    return 0;
  } else if (strcmp(name, "blinking_speed") == 0) {
    err = read_cb(cb_arg, &light_config.blinking_speed,
                  sizeof(light_config.blinking_speed));
    if (err < 0) {
      return err;
    }

    return 0;
  } else if (strcmp(name, "headlight_brightness") == 0) {
    err = read_cb(cb_arg, &light_config.headlight_brightness,
                  sizeof(light_config.headlight_brightness));
    if (err < 0) {
      return err;
    }

    return 0;
  } else if (strcmp(name, "brake_brightness") == 0) {
    err = read_cb(cb_arg, &light_config.brake_brightness,
                  sizeof(light_config.brake_brightness));
    if (err < 0) {
      return err;
    }

    return 0;
  } else if (strcmp(name, "blinker_brightness") == 0) {
    err = read_cb(cb_arg, &light_config.blinker_brightness,
                  sizeof(light_config.blinker_brightness));
    if (err < 0) {
      return err;
    }

    return 0;
  } else if (strcmp(name, "blinker_mode") == 0) {
    err = read_cb(cb_arg, &light_config.blinker_mode,
                  sizeof(light_config.blinker_mode));
    if (err < 0) {
      return err;
    }

    return 0;
  } else {
    return -ENOENT;
  }
}

struct settings_handler settings_config = {
    .name = "config",
    .h_get = handle_get,
    .h_set = handle_set,
    .h_commit = NULL,
    .h_export = NULL,
};

// Initialize the settings
int initialize_settings(void) {
  int err;

  err = settings_subsys_init();
  if (err) {
    LOG_ERR("Failed to initialize settings subsystem (err %d)", err);
    return err;
  }

  // Register handlers
  settings_register(&settings_config);

  err = settings_load();
  if (err) {
    LOG_ERR("Failed to load settings (err %d)", err);
    return err;
  }

  return 0;
};

// BLE Getters
ssize_t get_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/mode", &light_config.mode,
                             sizeof(light_config.mode));
  if (err < 0) {
    LOG_ERR("Failed to get mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &light_config.mode,
                           sizeof(light_config.mode));
};

// BLE Setters
ssize_t set_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags) {
  int err;

  if (len != sizeof(enum GlobalMode)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&light_config.mode, buf, len);

  // Save the setting
  err = settings_save_one("config/mode", &light_config.mode,
                          sizeof(light_config.mode));
  if (err) {
    LOG_ERR("Failed to save mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return len;
}
