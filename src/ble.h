#pragma once

// Custom services (DFU)
// UUID (Text): thin-ka-le-xd-fu0000000#
// UUID (HEX): 7468696e-6b61-6c65-7864-66750000000#
#define BT_UUID_DFU_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x7468696e, 0x6b61, 0x6c65, 0x7864, 0x66750000000)

#define BT_UUID_DFU_CHAR_VAL \
    BT_UUID_128_ENCODE(0x7468696e, 0x6b61, 0x6c65, 0x7864, 0x66750000001)

#define BT_UUID_DFU_SERVICE     BT_UUID_DECLARE_128(BT_UUID_DFU_SERVICE_VAL)
#define BT_UUID_DFU_CHAR        BT_UUID_DECLARE_128(BT_UUID_DFU_CHAR_VAL)

int start_ble(void);