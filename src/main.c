#include "ble.h"
#include "config.h"
#include <zephyr/kernel.h>

int main(void)
{
        initialize_settings();
        start_ble();

        int i = 0;
        while (true) {
                printk("Does BLE logging work? %d\n", i++);
                k_sleep(K_MSEC(1000));
        }
        return 0;
}
