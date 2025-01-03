#include "config.h"

#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

LOG_MODULE_REGISTER(config);

struct light_config config = {.mode = GLOBAL_OFF,
                              .blinking_speed = 500,
                              .headlight_brightness = 100,
                              .brake_idle_brightness = 50,
                              .brake_braking_brightness = 100,
                              .blinker_brightness = 100,
                              .brake_mode = BRAKE_IDLE,
                              .blinker_mode = BLINKER_OFF,
                              };

// Mutex for settings
K_MUTEX_DEFINE(config_mutex);
K_MSGQ_DEFINE(config_msgq, sizeof(struct light_config), 10, 4);

// Notify LED thread
static void notify_config_change(void) {
  // Send updated config to message queue
  k_mutex_lock(&config_mutex, K_FOREVER);
  k_msgq_put(&config_msgq, &config, K_NO_WAIT);
  k_mutex_unlock(&config_mutex);
}

// Handlers for Settings API
static int handle_get(const char *name, char *val, int val_len) {

  // Lock the mutex
  k_mutex_lock(&config_mutex, K_FOREVER);

  // Switch on the name of the setting
  if (!strcmp(name, "mode")) {

    if (val_len != sizeof(enum GlobalMode)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }
    memcpy(val, &config.mode, sizeof(enum GlobalMode));
    k_mutex_unlock(&config_mutex);
    return sizeof(enum GlobalMode);

  } else if (!strcmp(name, "blinking_speed")) {

    if (val_len != sizeof(int)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }

    memcpy(val, &config.blinking_speed, sizeof(int));
    k_mutex_unlock(&config_mutex);
    return sizeof(int);

  } else if (!strcmp(name, "headlight_brightness")) {

    if (val_len != sizeof(int)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }

    memcpy(val, &config.headlight_brightness, sizeof(int));
    k_mutex_unlock(&config_mutex);
    return sizeof(int);

  } else if (!strcmp(name, "brake_idle_brightness")) {

    if (val_len != sizeof(int)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }

    memcpy(val, &config.brake_idle_brightness, sizeof(int));
    k_mutex_unlock(&config_mutex);
    return sizeof(int);

  } else if (!strcmp(name, "brake_braking_brightness")) {

    if (val_len != sizeof(int)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }

    memcpy(val, &config.brake_braking_brightness, sizeof(int));
    k_mutex_unlock(&config_mutex);
    return sizeof(int);

  } else if (!strcmp(name, "blinker_brightness")) {

    if (val_len != sizeof(int)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }

    memcpy(val, &config.blinker_brightness, sizeof(int));
    k_mutex_unlock(&config_mutex);
    return sizeof(int);

  } else if (!strcmp(name, "brake_mode")) {

    if (val_len != sizeof(enum BrakeMode)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }

    memcpy(val, &config.brake_mode, sizeof(enum BrakeMode));
    k_mutex_unlock(&config_mutex);
    return sizeof(enum BrakeMode);
  } else if (!strcmp(name, "blinker_mode")) {

    if (val_len != sizeof(enum BlinkerMode)) {
      k_mutex_unlock(&config_mutex);
      return -EINVAL;
    }

    memcpy(val, &config.blinker_mode, sizeof(enum BlinkerMode));
    k_mutex_unlock(&config_mutex);
    return sizeof(enum BlinkerMode);
  }

  // Unlock the mutex
  k_mutex_unlock(&config_mutex);
  return -ENOENT;
}

static int handle_set(const char *name, size_t len, settings_read_cb read_cb,
                      void *cb_arg) {
  int err;
  LOG_INF("SETTING VALUE");

  // Lock the mutex
  k_mutex_lock(&config_mutex, K_FOREVER);

  if (strcmp(name, "mode") == 0) {
    err = read_cb(cb_arg, &config.mode, sizeof(config.mode));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else if (strcmp(name, "blinking_speed") == 0) {
    err =
        read_cb(cb_arg, &config.blinking_speed, sizeof(config.blinking_speed));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else if (strcmp(name, "headlight_brightness") == 0) {
    err = read_cb(cb_arg, &config.headlight_brightness,
                  sizeof(config.headlight_brightness));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else if (strcmp(name, "brake_idle_brightness") == 0) {
    err = read_cb(cb_arg, &config.brake_idle_brightness,
                  sizeof(config.brake_idle_brightness));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else if (strcmp(name, "brake_braking_brightness") == 0) {
    err = read_cb(cb_arg, &config.brake_braking_brightness,
                  sizeof(config.brake_braking_brightness));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else if (strcmp(name, "blinker_brightness") == 0) {
    err = read_cb(cb_arg, &config.blinker_brightness,
                  sizeof(config.blinker_brightness));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else if (strcmp(name, "brake_mode") == 0) {
    err = read_cb(cb_arg, &config.brake_mode, sizeof(config.brake_mode));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else if (strcmp(name, "blinker_mode") == 0) {
    err = read_cb(cb_arg, &config.blinker_mode, sizeof(config.blinker_mode));
    if (err < 0) {
      k_mutex_unlock(&config_mutex);
      return err;
    }

    k_mutex_unlock(&config_mutex);
    notify_config_change();
    return 0;
  } else {
    // Unlock the mutex
    k_mutex_unlock(&config_mutex);

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

  err = settings_runtime_get("config/mode", &config.mode, sizeof(config.mode));
  if (err < 0) {
    LOG_ERR("Failed to get mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &config.mode,
                           sizeof(config.mode));
};

ssize_t get_blinking_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                           void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/blinking_speed", &config.blinking_speed,
                             sizeof(config.blinking_speed));
  if (err < 0) {
    LOG_ERR("Failed to get blinking speed (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &config.blinking_speed,
                           sizeof(config.blinking_speed));
};

ssize_t get_headlight_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                 void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/headlight_brightness",
                             &config.headlight_brightness,
                             sizeof(config.headlight_brightness));
  if (err < 0) {
    LOG_ERR("Failed to get headlight brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &config.headlight_brightness,
                           sizeof(config.headlight_brightness));
};

ssize_t get_brake_idle_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                  void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/brake_idle_brightness",
                             &config.brake_idle_brightness,
                             sizeof(config.brake_idle_brightness));
  if (err < 0) {
    LOG_ERR("Failed to get brake idle brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &config.brake_idle_brightness,
                           sizeof(config.brake_idle_brightness));
};

ssize_t get_brake_braking_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                     void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/brake_braking_brightness",
                             &config.brake_braking_brightness,
                             sizeof(config.brake_braking_brightness));
  if (err < 0) {
    LOG_ERR("Failed to get brake braking brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &config.brake_braking_brightness,
                           sizeof(config.brake_braking_brightness));
};

ssize_t get_blinker_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/blinker_brightness",
                             &config.blinker_brightness,
                             sizeof(config.blinker_brightness));
  if (err < 0) {
    LOG_ERR("Failed to get blinker brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset,
                           &config.blinker_brightness,
                           sizeof(config.blinker_brightness));
};

ssize_t get_brake_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                       void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/brake_mode", &config.brake_mode,
                             sizeof(config.brake_mode));
  if (err < 0) {
    LOG_ERR("Failed to get brake mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &config.brake_mode,
                           sizeof(config.brake_mode));
};

ssize_t get_blinker_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset) {
  int err;

  err = settings_runtime_get("config/blinker_mode", &config.blinker_mode,
                             sizeof(config.blinker_mode));
  if (err < 0) {
    LOG_ERR("Failed to get blinker mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &config.blinker_mode,
                           sizeof(config.blinker_mode));
};

// BLE Setters
ssize_t set_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                 const void *buf, uint16_t len, uint16_t offset,
                 uint8_t flags) {
  int err;

  if (len != sizeof(enum GlobalMode)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.mode, buf, len);

  // Save the setting
  err = settings_save_one("config/mode", &config.mode, sizeof(config.mode));
  if (err) {
    LOG_ERR("Failed to save mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

    notify_config_change();
    LOG_INF("Mode set to %d", config.mode);

  return len;
}

ssize_t set_blinking_speed(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                           const void *buf, uint16_t len, uint16_t offset,
                           uint8_t flags) {
  int err;

  if (len != sizeof(int)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.blinking_speed, buf, len);

  // Save the setting
  err = settings_save_one("config/blinking_speed", &config.blinking_speed,
                          sizeof(config.blinking_speed));
  if (err) {
    LOG_ERR("Failed to save blinking speed (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

    notify_config_change();
    LOG_INF("Blinking speed set to %dms", config.blinking_speed);

  return len;
}

ssize_t set_headlight_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                 const void *buf, uint16_t len, uint16_t offset,
                                 uint8_t flags) {
  int err;

  if (len != sizeof(int)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.headlight_brightness, buf, len);

  // Save the setting
  err = settings_save_one("config/headlight_brightness", &config.headlight_brightness,
                          sizeof(config.headlight_brightness));
  if (err) {
    LOG_ERR("Failed to save headlight brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

    notify_config_change();
    LOG_INF("Headlight brightness set to %d%%", config.headlight_brightness);

  return len;
}

ssize_t set_brake_idle_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                  const void *buf, uint16_t len, uint16_t offset,
                                  uint8_t flags) {
  int err;

  if (len != sizeof(int)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.brake_idle_brightness, buf, len);

  // Save the setting
  err = settings_save_one("config/brake_idle_brightness", &config.brake_idle_brightness,
                          sizeof(config.brake_idle_brightness));
  if (err) {
    LOG_ERR("Failed to save brake idle brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

    notify_config_change();
    LOG_INF("Brake idle brightness set to %d%%", config.brake_idle_brightness);

  return len;
}

ssize_t set_brake_braking_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                     const void *buf, uint16_t len, uint16_t offset,
                                     uint8_t flags) {
  int err;

  if (len != sizeof(int)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.brake_braking_brightness, buf, len);

  // Save the setting
  err = settings_save_one("config/brake_braking_brightness", &config.brake_braking_brightness,
                          sizeof(config.brake_braking_brightness));
  if (err) {
    LOG_ERR("Failed to save brake braking brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

    notify_config_change();
    LOG_INF("Brake braking brightness set to %d%%", config.brake_braking_brightness);

  return len;
}

ssize_t set_blinker_brightness(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len, uint16_t offset,
                               uint8_t flags) {
  int err;

  if (len != sizeof(int)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.blinker_brightness, buf, len);

  // Save the setting
  err = settings_save_one("config/blinker_brightness", &config.blinker_brightness,
                          sizeof(config.blinker_brightness));
  if (err) {
    LOG_ERR("Failed to save blinker brightness (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

    notify_config_change();
    LOG_INF("Blinker brightness set to %d%%", config.blinker_brightness);

  return len;
}

ssize_t set_brake_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                       const void *buf, uint16_t len, uint16_t offset,
                       uint8_t flags) {
  int err;

  if (len != sizeof(enum BrakeMode)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.brake_mode, buf, len);

  // Save the setting
  err = settings_save_one("config/brake_mode", &config.brake_mode,
                          sizeof(config.brake_mode));
  if (err) {
    LOG_ERR("Failed to save brake mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

    notify_config_change();
    LOG_INF("Brake mode set to %d", config.brake_mode);

  return len;
}

ssize_t set_blinker_mode(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         const void *buf, uint16_t len, uint16_t offset,
                         uint8_t flags) {
  int err;

  if (len != sizeof(enum BlinkerMode)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(&config.blinker_mode, buf, len);

  // Save the setting
  err = settings_save_one("config/blinker_mode", &config.blinker_mode,
                          sizeof(config.blinker_mode));
  if (err) {
    LOG_ERR("Failed to save blinker mode (err %d)", err);
    return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
  }

  notify_config_change();
  LOG_INF("Blinker mode set to %d", config.blinker_mode);

  return len;
}
