#include "led_server.h"

////////////////////////////////////////////////////////////////////////////////

struct espconn led_server_conn;
esp_udp led_udp_server_conn;

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR led_server_rx_packet(void * arg, char* d, unsigned short len)
{
    uint8_t * data = (uint8_t*) d;

    if (len != 5) {
        os_printf("[LED] got invalid packet of length %d\n", len);
        return;
    }

    /*
    os_printf("[LED] set LED color to %02x:%02x:%02x:%02x:%02x\n",
            data[0], data[1], data[2], data[3], data[4]);
            */

    LED_set_arr(data);
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR LED_server_start(unsigned int local_port)
{
    // set up connections
    memset(&led_server_conn,     0, sizeof(led_server_conn));
    memset(&led_udp_server_conn, 0, sizeof(led_udp_server_conn));

    led_server_conn.type  = ESPCONN_UDP;
    led_server_conn.state = ESPCONN_NONE;
    led_server_conn.proto.udp = &led_udp_server_conn;
    led_server_conn.proto.udp->local_port = local_port;

    if (espconn_create(&led_server_conn)) {
        os_printf("[LED] ERROR: cannot create() connection.\n");
        return;
    }

    // start listening for UDP packets
    espconn_regist_recvcb(&led_server_conn, led_server_rx_packet);

    os_printf("[LED] listening for UDP packets on port %d\n", local_port);
}

////////////////////////////////////////////////////////////////////////////////
