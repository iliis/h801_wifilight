
/*
 * based on TimeNTP_ESP8266WiFi.ino
 * from https://raw.githubusercontent.com/PaulStoffregen/Time/master/examples/TimeNTP_ESP8266WiFi/TimeNTP_ESP8266WiFi.ino
 */

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <user_interface.h>
#include <espconn.h>
#include <ip_addr.h>

#include "simpletime.h"

// NTP Servers:
static const char ntpServerName[] = "ch.pool.ntp.org";
//static const char ntpServerName[] = "klamath.ch";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

unsigned int localPort = 8888;  // local port to listen for UDP packets

////////////////////////////////////////////////////////////////////////////////

void print_array(uint8* data, size_t len)
{
    size_t i = 0;
    for (; i < len; i++) {
        os_printf("%02x", data[i]);
        if (i % 16 == 15)
            os_printf("\n");
        else
            os_printf(":");
    }
    os_printf("\n");
}

////////////////////////////////////////////////////////////////////////////////

ip_addr_t ntpServerIP; // cached IP of NTP server name

////////////////////////////////////////////////////////////////////////////////
// DNS LOOKUP
////////////////////////////////////////////////////////////////////////////////

struct espconn dns_connection;
esp_tcp dns_tcp_connection;

////////////////////////////////////////////////////////////////////////////////

#define IP322STR(ip)    ((ip) >> 0*8)&0xFF, ((ip) >> 1*8)&0xFF, ((ip) >> 2*8)&0xFF, ((ip) >> 3*8)&0xFF

void ICACHE_FLASH_ATTR NTPnamelookup_cb(const char *name, ip_addr_t *ipaddr, void *arg)
{
    if (!ipaddr) {
        os_printf("[DNS] ERROR: callback: cannot lookup NTP server IP.\n");
        return;
    }

    memcpy(&ntpServerIP, ipaddr, sizeof(ip_addr_t));
    os_printf("[DNS] got IP address of NTP server at %s: " IPSTR "\n", name, IP322STR(ntpServerIP.addr));
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR NTP_start_name_lookup()
{
    memset(&dns_connection,     0, sizeof(dns_connection));
    memset(&dns_tcp_connection, 0, sizeof(dns_tcp_connection));

    dns_connection.type  = ESPCONN_TCP;
    dns_connection.state = ESPCONN_NONE;
    dns_connection.proto.tcp = &dns_tcp_connection;

    switch (espconn_gethostbyname(&dns_connection, ntpServerName, &ntpServerIP, NTPnamelookup_cb))
    {
        case ESPCONN_OK:
            os_printf("[DNS] lookup success, name already in local cache.\n");
            break;

        case ESPCONN_INPROGRESS:
            os_printf("[DNS] lookup in progress, waiting for callback...\n");
            //os_printf("[DNS] connection state: %d\n", dns_connection.state);
            break;

        case ESPCONN_ARG:
            os_printf("[DNS] ERROR: Lookupd failed. DNS client not initialized or invalid hostname.\n");
            break;

        default:
            os_printf("[DNS] ERROR: unknown return value from gethostbyname()\n");
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// NTP
////////////////////////////////////////////////////////////////////////////////

struct espconn ntp_rx_connection;
esp_udp ntp_udp_rx_connection;

struct espconn ntp_tx_connection;
esp_udp ntp_udp_tx_connection;

#define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message
uint8 packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR sendNTPpacket()
{
    if (ntpServerIP.addr == 0) {
        os_printf("[NTP] ERROR: Server IP not set.\n");
        return;
    }

    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:

    // we need a new connection, cannot use receiving one
    // destination IP and port need to be set before every call to
    // espconn_send() according to some example code I found...

    // set up connections
    /*
    memset(&ntp_tx_connection,     0, sizeof(ntp_tx_connection));
    memset(&ntp_udp_tx_connection, 0, sizeof(ntp_udp_tx_connection));

    ntp_tx_connection.type  = ESPCONN_UDP;
    ntp_tx_connection.state = ESPCONN_NONE;
    ntp_tx_connection.proto.udp = &ntp_udp_tx_connection;
    ntp_tx_connection.proto.udp->local_port = localPort+1;
    memcpy(ntp_tx_connection.proto.udp->remote_ip, &ntpServerIP.addr, 4);
    ntp_tx_connection.proto.udp->remote_port = 123; // NTP requests are to port 123
    */
    memcpy(ntp_rx_connection.proto.udp->remote_ip, &ntpServerIP.addr, 4);
    ntp_rx_connection.proto.udp->remote_port = 123; // NTP requests are to port 123

    // actually send the packet
    /*
    if (espconn_create(&ntp_rx_connection))
    {
        os_printf("[NTP] transmit packet: failed to create() connection.\n");
        return;
    }
    */
    int r = espconn_send(&ntp_rx_connection, packetBuffer, NTP_PACKET_SIZE);
    //espconn_delete(&ntp_rx_connection);

    os_printf("[NTP] send NTP request to "IPSTR" (ret: %d)\n", IP2STR(ntp_rx_connection.proto.udp->remote_ip), r);
}

////////////////////////////////////////////////////////////////////////////////

// number of seconds between 1900 and 1970
#define DIFF_SEC_1900_1970 (2208988800UL)

void ICACHE_FLASH_ATTR ntp_rx_packet(void * arg, char* data, unsigned short len)
{
    struct espconn * conn = (struct espconn *) arg;

    if (len != NTP_PACKET_SIZE) {
        os_printf("[NTP] got invalid packet from server:\n");
        print_array(data, len);
        return;
    }

    // TODO: parse packet and handle errors, kiss-of-death etc.

    // get seconds since 1.1.1900
    unsigned long s = 0;
    s  = data[40]; s <<= 8;
    s |= data[41]; s <<= 8;
    s |= data[42]; s <<= 8;
    s |= data[43];

    os_printf("[NTP] got data from server at " IPSTR ": %lu\n", IP2STR(conn->proto.udp->remote_ip), s);

    time_t t = s - DIFF_SEC_1900_1970;

    tmElements_t tm;
    simple_localtime(t, &tm);

    os_printf("[NTP] current time: %02d:%02d.%02d UTC\n", tm.Hour, tm.Minute, tm.Second);
}

////////////////////////////////////////////////////////////////////////////////

static volatile os_timer_t ntp_update_timer;

void ntp_update(void *arg)
{
    sendNTPpacket();
}

////////////////////////////////////////////////////////////////////////////////

static volatile os_timer_t reboot_timer;
void reboot_timer_cb(void * arg)
{
    os_printf("REBOOTING!\n");
    system_restart();
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR NTPinit()
{
    //os_timer_setfn(&reboot_timer, reboot_timer_cb, NULL);
    //os_timer_arm(&reboot_timer, 15*1000, 0); // reboot after 15 sec


    // look up server name
    NTP_start_name_lookup();

    // set up connections
    memset(&ntp_rx_connection,     0, sizeof(ntp_rx_connection));
    memset(&ntp_udp_rx_connection, 0, sizeof(ntp_udp_rx_connection));

    ntp_rx_connection.type  = ESPCONN_UDP;
    ntp_rx_connection.state = ESPCONN_NONE;
    ntp_rx_connection.proto.udp = &ntp_udp_rx_connection;
    ntp_rx_connection.proto.udp->local_port = localPort;

    if (espconn_create(&ntp_rx_connection)) {
        os_printf("[NTP] ERROR: cannot create() connection.\n");
        return;
    }

    os_printf("[NTP] listening to UDP packets on port %d\n", ntp_rx_connection.proto.udp->local_port);
    espconn_regist_recvcb(&ntp_rx_connection, ntp_rx_packet);

    // start listening for UDP packets
    //espconn_accept(&ntp_rx_connection);

    // periodically call ntp_update()
    os_timer_disarm(&ntp_update_timer);
    os_timer_setfn(&ntp_update_timer, ntp_update, NULL);
    os_timer_arm(&ntp_update_timer, 2000, 1); // repeat every other second
}

////////////////////////////////////////////////////////////////////////////////
