#include "common.h"
#include "types.h"
#include "i2c.h"
#include "ssd1306.h"

// ----- Channel hop timer variables

#define CHANNEL_HOP_INTERVAL 1000
static volatile os_timer_t channelHop_timer;

static int packets = 0;

// ----- Function prototypes

void ICACHE_FLASH_ATTR start_process();
void ICACHE_FLASH_ATTR promisc_cb(uint8 *buf, uint16 len);

void channelHop(void *arg)
{
    // 1 - 13 channel hopping
    uint8 new_channel = wifi_get_channel() % 12 + 1;
    wifi_set_channel(new_channel);
    packets = 0;
}

void ICACHE_FLASH_ATTR promisc_cb(uint8 *buf, uint16 len)
{
    if (packets < 50) {

        ssd1306_clear(0);

        // draw the header with the channel #
        size_t header_size = 20;
        char *header = (char *)os_zalloc(header_size * sizeof(char));
        os_sprintf(header, "Sniffing Channel: %d", wifi_get_channel());
        ssd1306_draw_string(0, 0, 2, header, SSD1306_COLOR_WHITE, SSD1306_COLOR_BLACK);
        os_free(header);

        size_t i;
        
        uint8_t xPos = 0;
        uint8_t yPos = 16;
        uint8_t line = 1;

        for (i = 0; i < len; ++i) {

            if (buf[i] == 0x00 || buf[i] == 0xFF) {
                continue;
            }

            char *hex = (char *)os_zalloc(2 * sizeof(char));

            os_sprintf(hex, "%02x\0", buf[i]);

            uint8_t width = ssd1306_measure_string(0, hex) + 2;
            
            if (xPos + width > ssd1306_get_width(0)) {
                yPos += ssd1306_get_font_height(0);
                xPos = 0;
                line++;
            }

            if (line < 5) {
                ssd1306_draw_string(0, xPos, yPos, hex, SSD1306_COLOR_WHITE, SSD1306_COLOR_BLACK);
            }

            xPos += width;
            os_free(hex);
        }

        ssd1306_refresh(0, false);

        packets++;
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

void ICACHE_FLASH_ATTR system_init_done()
{  
  os_printf(" -> System Init finished!\n\n");
  start_hopping();
}

//Init function 
void ICACHE_FLASH_ATTR user_init()
{
    uart_div_modify(0, UART_CLK_FREQ / 115200);

    os_printf("*** Monitor mode test ***\r\n");

    os_printf(" -> Set opmode ... ");
    wifi_set_opmode(STATION_MODE);
    os_printf("done.\n");

    os_printf(" -> Initialize i2c ... ");
    i2c_init();
    os_printf("done.\n");

    os_printf(" -> Initialize SSD1306 ... ");
    if (ssd1306_init(0)) {
        ssd1306_select_font(0, 1);
        os_printf("done.\n");
    }
    else {
        os_printf("failed.\n");
    }
    

    os_printf(" -> User Init finished!\n\n");

    system_init_done_cb(system_init_done);
}