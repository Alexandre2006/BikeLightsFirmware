#include "ble.h"
#include <zephyr/kernel.h>

int main(void)
{
        start_ble();

        int i = 0;
        while (true) {
                printk("Does BLE logging work? %d\n", i++);
                k_sleep(K_MSEC(1000));
        }
        return 0;
}
