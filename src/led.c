#include "led.h"
#include "config.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/led.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(led);

#define LED_PWM_NODE_ID	DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds) // 0: Headlight, 1: Brake, 2: Left Blinker, 3: Right Blinker
const struct device *led_pwm;

struct light_config received_config = {
    .mode = GLOBAL_OFF,
    .blinking_speed = 500,
    .headlight_brightness = 100,
    .brake_idle_brightness = 50,
    .brake_braking_brightness = 100,
    .blinker_brightness = 100,
    .brake_mode = BRAKE_IDLE,
    .blinker_mode = BLINKER_OFF,
};

atomic_t left_blinker_on = ATOMIC_INIT(0);
atomic_t right_blinker_on = ATOMIC_INIT(0);
atomic_t hazards_on = ATOMIC_INIT(0);
bool last_blinker_state = false;
static K_TIMER_DEFINE(blink_timer, NULL, NULL);

int initialize_leds(void) {
    led_pwm = DEVICE_DT_GET(LED_PWM_NODE_ID);

    if (!device_is_ready(led_pwm)) {
        LOG_ERR("LED PWM device not ready");
        return -ENODEV;
    }

    return 0;
}

void updateHeadlight(struct light_config config) {
    // Check if light should be on
    if (config.mode == GLOBAL_OFF || config.mode == GLOBAL_REAR_ONLY) {
        // Turn off headlight
        led_set_brightness(led_pwm, 0, 0);
        LOG_INF("Headlight off");
    } else {
        // Turn on headlight
        led_set_brightness(led_pwm, 0, config.headlight_brightness);
        LOG_INF("Headlight on (brightness: %d)", config.headlight_brightness);
    }
}

void updateBrake(struct light_config config) {
    if (config.mode == GLOBAL_OFF) {
        // Turn off brake light
        led_set_brightness(led_pwm, 1, 0);
        LOG_INF("Brake light off");
    } else if (config.brake_mode == BRAKE_IDLE) {
        // Turn on brake light (idle)
        led_set_brightness(led_pwm, 1, config.brake_idle_brightness);
        LOG_INF("Brake light on (idle) (brightness: %d)", config.brake_idle_brightness);
    } else {
        // Turn on brake light (braking)
        led_set_brightness(led_pwm, 1, config.brake_braking_brightness);
        LOG_INF("Brake light on (braking) (brightness: %d)", config.brake_braking_brightness);
    }
}

void config_receiver_thread() {
    while (true) {
        // Wait for update
        k_msgq_get(&config_msgq, &received_config, K_FOREVER);

        // Update headlights
        updateHeadlight(received_config);

        // Update brakes (overriden by hazards)
        if (!atomic_get(&hazards_on)) {
            updateBrake(received_config);
        }

        // Update blinking lights
        atomic_set(&left_blinker_on, received_config.blinker_mode == BLINKER_LEFT);
        atomic_set(&right_blinker_on, received_config.blinker_mode == BLINKER_RIGHT);
        atomic_set(&hazards_on, received_config.blinker_mode == BLINKER_HAZARD);

        // Log changes (blinking lights)
        if (atomic_get(&left_blinker_on)) {
            LOG_INF("Left blinker on");
        } else {
            LOG_INF("Left blinker off");
        }

        if (atomic_get(&right_blinker_on)) {
            LOG_INF("Right blinker on");
        } else {
            LOG_INF("Right blinker off");
        }

        if (atomic_get(&hazards_on)) {
            LOG_INF("Hazards on");
        } else {
            LOG_INF("Hazards off");
        }
    }
}

void blinking_thread() {
    while (true) {
        // Sync with config receiver
        k_timer_status_sync(&blink_timer);
        k_timer_start(&blink_timer, K_MSEC(received_config.blinking_speed), K_FOREVER);

        // Update left blinkers (on/hazards)
        if (atomic_get(&left_blinker_on) || atomic_get(&hazards_on)) {
            led_set_brightness(led_pwm, 2, last_blinker_state ? received_config.blinker_brightness : 0);
        } else {
            led_set_brightness(led_pwm, 2, 0);
        }

        // Update right blink (on/hazards)
        if (atomic_get(&right_blinker_on) || atomic_get(&hazards_on)) {
            led_set_brightness(led_pwm, 3, last_blinker_state ? received_config.blinker_brightness : 0);
        } else {
            led_set_brightness(led_pwm, 3, 0);
        }

        // Update brakes (hazards)
        if (atomic_get(&hazards_on)) {
            led_set_brightness(led_pwm, 1, last_blinker_state ? received_config.blinker_brightness : 0);
        }

        // Update last blinker state
        last_blinker_state = !last_blinker_state;
    }
}

K_THREAD_DEFINE(config_receiver_thread_id, 1024, config_receiver_thread, NULL, NULL, NULL, 10, 0, 0);
K_THREAD_DEFINE(blinking_thread_id, 1024, blinking_thread, NULL, NULL, NULL, 10, 0, 0);