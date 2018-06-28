#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <os_type.h>
#include <user_interface.h>
#include <espconn.h>
#include <driver/uart.h>
//#include "driver/uart.h"

#include "user_config.h"
#include "ntp.h"
#include "led_server.h"
#include "animation_presets.h"
#include "alarm.h"

const char* wifi_disconnect_reason_str(uint8_t reason)
{
    switch (reason) {
        case REASON_UNSPECIFIED:
            return "REASON_UNSPECIFIED";

        case REASON_AUTH_EXPIRE:
            return "REASON_AUTH_EXPIRE";

        case REASON_AUTH_LEAVE:
            return "REASON_AUTH_LEAVE";

        case REASON_ASSOC_EXPIRE:
            return "REASON_ASSOC_EXPIRE";

        case REASON_ASSOC_TOOMANY:
            return "REASON_ASSOC_TOOMANY";

        case REASON_NOT_AUTHED:
            return "REASON_NOT_AUTHED";

        case REASON_NOT_ASSOCED:
            return "REASON_NOT_ASSOCED";

        case REASON_ASSOC_LEAVE:
            return "REASON_ASSOC_LEAVE";

        case REASON_ASSOC_NOT_AUTHED:
            return "REASON_ASSOC_NOT_AUTHED";

        case REASON_DISASSOC_PWRCAP_BAD:
            return "REASON_DISASSOC_PWRCAP_BAD";

        case REASON_DISASSOC_SUPCHAN_BAD:
            return "REASON_DISASSOC_SUPCHAN_BAD";

        case REASON_IE_INVALID:
            return "REASON_IE_INVALID";

        case REASON_MIC_FAILURE:
            return "REASON_MIC_FAILURE";

        case REASON_4WAY_HANDSHAKE_TIMEOUT:
            return "REASON_4WAY_HANDSHAKE_TIMEOUT";

        case REASON_GROUP_KEY_UPDATE_TIMEOUT:
            return "REASON_GROUP_KEY_UPDATE_TIMEOUT";

        case REASON_IE_IN_4WAY_DIFFERS:
            return "REASON_IE_IN_4WAY_DIFFERS";

        case REASON_GROUP_CIPHER_INVALID:
            return "REASON_GROUP_CIPHER_INVALID";

        case REASON_PAIRWISE_CIPHER_INVALID:
            return "REASON_PAIRWISE_CIPHER_INVALID";

        case REASON_AKMP_INVALID:
            return "REASON_AKMP_INVALID";

        case REASON_UNSUPP_RSN_IE_VERSION:
            return "REASON_UNSUPP_RSN_IE_VERSION";

        case REASON_INVALID_RSN_IE_CAP:
            return "REASON_INVALID_RSN_IE_CAP";

        case REASON_802_1X_AUTH_FAILED:
            return "REASON_802_1X_AUTH_FAILED";

        case REASON_CIPHER_SUITE_REJECTED:
            return "REASON_CIPHER_SUITE_REJECTED";


        case REASON_BEACON_TIMEOUT:
            return "REASON_BEACON_TIMEOUT";

        case REASON_NO_AP_FOUND:
            return "REASON_NO_AP_FOUND";

        case REASON_AUTH_FAIL:
            return "REASON_AUTH_FAIL";

        case REASON_ASSOC_FAIL:
            return "REASON_ASSOC_FAIL";

        case REASON_HANDSHAKE_TIMEOUT:
            return "REASON_HANDSHAKE_TIMEOUT";

        default:
            return "INVALID REASON";
    }
}

void wifi_callback( System_Event_t *evt )
{
    os_printf( "%s: %d\n", __FUNCTION__, evt->event );

    switch ( evt->event )
    {
        case EVENT_STAMODE_CONNECTED:
            os_printf("connect to ssid %s, channel %d\n",
                    evt->event_info.connected.ssid,
                    evt->event_info.connected.channel);
            break;

        case EVENT_STAMODE_DISCONNECTED:
            os_printf("disconnect from ssid %s, reason %s\n",
                    evt->event_info.disconnected.ssid,
                    wifi_disconnect_reason_str(evt->event_info.disconnected.reason));

            LED_set(100,30,0, 0,0);
            //deep_sleep_set_option( 0 );
            //system_deep_sleep( 60 * 1000 * 1000 );  // 60 seconds
            //LED_set(255,0,0, 0,0);
            break;

        case EVENT_STAMODE_GOT_IP:
            os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
                    IP2STR(&evt->event_info.got_ip.ip),
                    IP2STR(&evt->event_info.got_ip.mask),
                    IP2STR(&evt->event_info.got_ip.gw));
            os_printf("\n");

            LED_set(0,155,0,0,0); // green: connected

            // initialize NPT as soon as we have a working connection
            NTPinit();
            break;

        default:
            break;
    }
}


////////////////////////////////////////////////////////////////////////////////
// SERVER
////////////////////////////////////////////////////////////////////////////////

struct espconn server_connection;
esp_tcp server_tcp_connection;

void ICACHE_FLASH_ATTR server_rx_data_cb(void * arg, char* data, unsigned short len)
{
    os_printf("got data: %d bytes\n", len);

    unsigned short i = 0;
    for (; i < len; i++) {
        os_printf("%c", data[i]);
    }
    os_printf("\n");
}

void ICACHE_FLASH_ATTR server_connection_closed_cb(void * arg)
{
    os_printf("connection closed\n");
}

void ICACHE_FLASH_ATTR server_accept_connect_cb(void * arg)
{
    struct espconn * conn = (struct espconn *) arg;

    os_printf("got incoming connection from " IPSTR "\n", IP2STR(conn->proto.tcp->remote_ip));

    //espconn_regist_recvcb(conn, server_rx_data_cb);
    //espconn_regist_disconcb(conn, server_connection_closed_cb);

    char greeting[] = "Hallo :)\n";
    espconn_sent(conn, greeting, sizeof(greeting));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//Init function 
void ICACHE_FLASH_ATTR
user_init()
{
    // Configure the UART
    //uart_init(115200, 115200);
    uart_init(74880, 74880); // same as bootloader
    // enable system messages

    //uart_init(115200, 115200);
    //uart_div_modify(0, UART_CLK_FREQ / 115200); // initialize UART0
    //uart_div_modify(1, UART_CLK_FREQ / 115200); // initialize UART1
    UART_SetPrintPort(1);
    system_set_os_print(1);

    /*
    while (1) {
        uint32_t i = 0;
        os_printf("hallo welt\n");
        //uart1_sendStr_no_wait("hallo welt :)\n");
        for (; i<100; i++) {
            os_delay_us(10000);
        }
    }
    */

    ///////////////////////////////////////////////////////////////////////////
    // RGB LED output

    LED_init();
    LED_set(100,0,0,0,0);

    ///////////////////////////////////////////////////////////////////////////
    // CONNECT

    struct station_config stationConf = {
        .ssid = SSID,
        .password = SSID_PASSWORD
    };

    //Set station mode
    wifi_set_opmode(STATION_MODE); // client instead of SOFTAP or STATIONAP

    //Set ap settings
    wifi_station_set_config(&stationConf);

    wifi_set_event_handler_cb( wifi_callback );

    ///////////////////////////////////////////////////////////////////////////
    // BLINK

    //Set GPIO2 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);

    //Set GPIO2 low
    gpio_output_set(0, BIT5, BIT5, 0);

    os_printf("Booting up WiFiLi\n");

    ///////////////////////////////////////////////////////////////////////////
    // START SERVER

    memset(&server_connection,     0, sizeof(server_connection));
    memset(&server_tcp_connection, 0, sizeof(server_tcp_connection));

    // set up server
    espconn_create(&server_connection);
    server_connection.type  = ESPCONN_TCP;
    server_connection.state = ESPCONN_NONE;
    server_connection.proto.tcp = &server_tcp_connection;
    server_connection.proto.tcp->local_port = 41;

    // set callback for incoming connections
    espconn_regist_connectcb(&server_connection, server_accept_connect_cb);

    // can we set the callbacks already here?
    espconn_regist_recvcb(&server_connection, server_rx_data_cb);
    espconn_regist_disconcb(&server_connection, server_connection_closed_cb);

    // start listening
    espconn_accept(&server_connection);

    // set TCP timeout
    espconn_regist_time(&server_connection, 15, 0);

    ///////////////////////////////////////////////////////////////////////////

    LED_server_start(444);

    alarm_server_init(1);

    // this doesn't, it *does* boot into bootloader but then flashing fails
    //os_printf("RESTARTING!\n");
    //uart_init(8*115200, 8*115200); // set UART to 912600 baud
    //system_restart();
    //
}
