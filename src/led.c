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

void updateBlinkers(struct light_config config) {
    if (config.mode == GLOBAL_OFF || config.blinker_mode == BLINKER_OFF) {
        // Turn off blinkers
        led_set_brightness(led_pwm, 2, 0);
        led_set_brightness(led_pwm, 3, 0);
        LOG_INF("Blinkers off");
    } else if (config.blinker_mode == BLINKER_LEFT) {
        // Set brightness for left blinker & blink
        led_set_brightness(led_pwm, 2, config.blinker_brightness);
        led_blink(led_pwm, 2, config.blinking_speed, config.blinking_speed);

        // Turn off right blinker
        led_set_brightness(led_pwm, 3, 0);
    } else if (config.blinker_mode == BLINKER_RIGHT) {
        // Set brightness for the right blinker & blink
        led_set_brightness(led_pwm, 3, config.blinker_brightness);
        led_blink(led_pwm, 3, config.blinking_speed, config.blinking_speed);

        // Turn off left blinker
        led_set_brightness(led_pwm, 2, 0);
    }
}

void updateHazards(struct light_config config) {
    // Set brightness of brakes and blinkers
    led_set_brightness(led_pwm, 1, config.brake_braking_brightness);
    led_set_brightness(led_pwm, 2, config.blinker_brightness);
    led_set_brightness(led_pwm, 3, config.blinker_brightness);

    // Blink all lights
    led_blink(led_pwm, 1, config.blinking_speed, config.blinking_speed);
    led_blink(led_pwm, 2, config.blinking_speed, config.blinking_speed);
    led_blink(led_pwm, 3, config.blinking_speed, config.blinking_speed);

    LOG_INF("Hazards on (brake brightness: %d, blinker brightness: %d, speed: %d)", config.brake_braking_brightness, config.blinker_brightness, config.blinking_speed);
}

void config_receiver_thread() {
    struct light_config received_config;

    while (true) {
        // Wait for update
        k_msgq_get(&config_msgq, &received_config, K_FOREVER);

        LOG_INF("RECEIVED CHANGE");

        // Update headlight
        updateHeadlight(received_config);

        // Update rear lights
        if (received_config.blinker_mode == BLINKER_HAZARD) {
            updateHazards(received_config);
        } else {
            updateBrake(received_config);
            updateBlinkers(received_config);
        }    
    }
}

K_THREAD_DEFINE(config_receiver_thread_id, 1024, config_receiver_thread, NULL, NULL, NULL, 10, 0, 0);