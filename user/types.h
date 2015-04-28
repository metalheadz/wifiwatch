#include "user_interface.h"

#ifndef _TYPES_H_
#define _TYPES_H_

typedef enum _encrytion_mode {
    ENCRY_NONE           = 1,
    ENCRY_WEP,
    ENCRY_TKIP,
    ENCRY_CCMP
} ENCYTPTION_MODE;

struct router_info {
    SLIST_ENTRY(router_info)     next;

    u8 bssid[6];
    u8 ssid[33];
    u8 channel;
    u8 authmode;

    u16 rx_seq;
    u8 encrytion_mode;
    u8 iv[8];
    u8 iv_check;
};

#endif