#include "ble.h"
#include "config.h"
#include "led.h"
#include <zephyr/kernel.h>

int main(void)
{
        initialize_leds();
        initialize_settings();
        start_ble();

        return 0;
}
