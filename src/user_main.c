#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <os_type.h>
#include <user_interface.h>
#include <espconn.h>
//#include "driver/uart.h"

#include "user_config.h"
#include "ntp.h"
#include "led_server.h"

static volatile os_timer_t some_timer;

void some_timerfunc(void *arg)
{
    //Do blinky stuff
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT5)
    {
        //Set GPIO2 to LOW
        gpio_output_set(0, BIT5, BIT5, 0);
    }
    else
    {
        //Set GPIO2 to HIGH
        gpio_output_set(BIT5, 0, BIT5, 0);
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
            os_printf("disconnect from ssid %s, reason %d\n",
                    evt->event_info.disconnected.ssid,
                    evt->event_info.disconnected.reason);

            deep_sleep_set_option( 0 );
            system_deep_sleep( 60 * 1000 * 1000 );  // 60 seconds
            break;

        case EVENT_STAMODE_GOT_IP:
            os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
                    IP2STR(&evt->event_info.got_ip.ip),
                    IP2STR(&evt->event_info.got_ip.mask),
                    IP2STR(&evt->event_info.got_ip.gw));
            os_printf("\n");

            LED_set(0,255,0,0,0); // green: connected

            // initialize NPT as soon as we have a working connection
            //NTPinit();
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

extern void __real_system_restart_local();


typedef void (*voidfunc_t)(void);

//Init function 
void ICACHE_FLASH_ATTR
user_init()
{
    // Configure the UART
    uart_init(115200, 115200);
    // enable system messages

    //uart_init(115200, 115200);
    //uart_div_modify(0, UART_CLK_FREQ / 115200); // initialize UART0
    //uart_div_modify(1, UART_CLK_FREQ / 115200); // initialize UART1
    UART_SetPrintPort(1);
    system_set_os_print(1);

    ///////////////////////////////////////////////////////////////////////////
    // RGB LED output

    LED_init();

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

    os_printf("this is user_init()!\n");

    //Disarm timer
    os_timer_disarm(&some_timer);

    //Setup timer
    os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //1000 is the fire time in ms
    //0 for once and 1 for repeating
    //os_timer_arm(&some_timer, 1000, 1);
    os_timer_arm(&some_timer, 1000*2, 1);

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

    // this doesn't, it *does* boot into bootloader but then flashing fails
    //os_printf("RESTARTING!\n");
    //uart_init(8*115200, 8*115200); // set UART to 912600 baud
    //system_restart();
}
