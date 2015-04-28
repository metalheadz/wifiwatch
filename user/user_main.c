#include "ets_sys.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"

#include "types.h"

// ----- Channel hop timer variables

#define CHANNEL_HOP_INTERVAL 1000
static volatile os_timer_t channelHop_timer;

// ----- Network list timer variables

SLIST_HEAD(router_info_head, router_info) router_list;

// ----- Function prototypes

void ICACHE_FLASH_ATTR start_process();
void ICACHE_FLASH_ATTR promisc_cb(uint8 *buf, uint16 len);


void printmac(char* buf, unsigned int o)
{
 if(buf[o+4] == 0x00 && buf[o+5] == 0x00)
  return;
if(buf[o+4] == 0xff && buf[o+5] == 0xff)
  return;
int i;
for(i=0;i<6;i++)
  if(buf[o+i] != 0x00 && buf[o+i] != 0xff)
   goto good;
return;
good: ;
   // char mac[strlen("00:00:00:00:00:00\n")];
os_printf("%02x:%02x:%02x:%02x:%02x:%02x\n", buf[o+0], buf[o+1], buf[o+2], buf[o+3], buf[o+4], buf[o+5]);
   // uart0_sendStr(mac);
}

void channelHop(void *arg)
{
    // 1 - 13 channel hopping
    uint8 new_channel = wifi_get_channel() % 12 + 1;
    os_printf("** hop to %d **\n", new_channel);
    wifi_set_channel(new_channel);
}

void ICACHE_FLASH_ATTR promisc_cb(uint8 *buf, uint16 len)
{
    // os_printf("-> %3d: %d\n", wifi_get_channel(), len);
    // printmac(buf,  4);
    // printmac(buf, 10);
    // printmac(buf, 16);

    // os_printf("\n");

    if ((buf[1] & 0x01) == 0x01) {  // just toDS
        // printmac(buf,  4);
        // os_printf("\n");

    uint8 *mac = buf + 4;

    struct router_info *info = NULL;
    SLIST_FOREACH(info, &router_list, next) {

            // os_printf("comparing: ");
            // printmac(info->bssid, 0);
            // os_printf(" to ");
            // printmac(mac, 0);
            // os_printf("\n");

        if (os_memcmp(mac + 12, info->bssid, 6) == 0) {
            os_printf("1 Found SSID: %s\n", info->ssid);
        }
        else if (os_memcmp(mac + 12, info->bssid, 6) == 0) {
            os_printf("2 Found SSID: %s\n", info->ssid);
        }
        else if (os_memcmp(mac + 12, info->bssid, 6) == 0) {
            os_printf("3 Found SSID: %s\n", info->ssid);
        }
    }

        // printmac(buf, 10);
        // printmac(buf, 16);
    }

}

void ICACHE_FLASH_ATTR start_hopping()
{
    os_printf(" -> Promisc mode setup ... ");
    wifi_promiscuous_enable(1);
    wifi_set_promiscuous_rx_cb(promisc_cb);
    os_printf("done.\n");

    os_printf(" -> Timer setup ... ");
    os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
    os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL, 1);
    os_printf("done.\n");
}

void ICACHE_FLASH_ATTR clear_router_list()
{
    struct router_info *info = NULL;

    while((info = SLIST_FIRST(&router_list)) != NULL){
        SLIST_REMOVE_HEAD(&router_list, next);
        os_free(info);
    }
}

void ICACHE_FLASH_ATTR wifi_scan_done(void *arg, STATUS status)
{
    if (status == OK) {

        // Clear the old list

        clear_router_list();

        // Populate the new list

        struct bss_info *bss = (struct bss_info *)arg;

        while (bss != NULL) {

            if (bss->channel != 0) {
                struct router_info *info = NULL;

                // os_printf("ssid %s, channel %d, authmode %d, rssi %d\n", ssid, bss->channel, bss->authmode, bss->rssi);

                info = (struct router_info *)os_zalloc(sizeof(struct router_info));
                info->authmode = bss->authmode;
                info->channel = bss->channel;

                os_memcpy(info->bssid, bss->bssid, 6);

                os_memset(info->ssid, 0, 33);

                if (os_strlen(bss->ssid) <= 32) {
                    os_memcpy(info->ssid, bss->ssid, os_strlen(bss->ssid));
                } else {
                    os_memcpy(info->ssid, bss->ssid, 32);
                }

                SLIST_INSERT_HEAD(&router_list, info, next);
            }
            bss = STAILQ_NEXT(bss, next);
        }

        // Start sniffin'

        start_hopping();
    }
    else {
        start_process();
    }
}

void ICACHE_FLASH_ATTR start_process()
{
    os_timer_disarm(&channelHop_timer);
    wifi_station_scan(NULL,wifi_scan_done);
}

void ICACHE_FLASH_ATTR system_init_done()
{  
  os_printf(" -> System Init finished!\n\n");
  start_process();
}

//Init function 
void ICACHE_FLASH_ATTR user_init()
{
    uart_div_modify(0, UART_CLK_FREQ / 9600);


    os_printf("*** Monitor mode test ***\r\n");

    os_printf(" -> Set opmode ... ");
    wifi_set_opmode(STATION_MODE);
    os_printf("done.\n");

    os_printf(" -> User Init finished!\n\n");

    system_init_done_cb(system_init_done);
}