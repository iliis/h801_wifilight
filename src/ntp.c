
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

ip_addr_t ntpServerIP; // cached IP of NTP server name


struct espconn dns_connection;
esp_tcp dns_tcp_connection;

////////////////////////////////////////////////////////////////////////////////

#define IP322STR(ip)    ((ip) >> 0*8)&0xFF, ((ip) >> 1*8)&0xFF, ((ip) >> 2*8)&0xFF, ((ip) >> 3*8)&0xFF

void NTPnamelookup_cb(const char *name, ip_addr_t *ipaddr, void *arg)
{
    if (!ipaddr) {
        os_printf("[DNS] ERROR: callback: cannot lookup NTP server IP.\n");
        return;
    }

    memcpy(&ntpServerIP, ipaddr, sizeof(ip_addr_t));
    os_printf("[DNS] got IP address of NTP server at %s: " IPSTR "\n", name, IP322STR(ntpServerIP.addr));
}

////////////////////////////////////////////////////////////////////////////////

void NTP_start_name_lookup()
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
            os_printf("[DNS] connection state: %d\n", dns_connection.state);
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

void NTPinit()
{
    // look up server name
    NTP_start_name_lookup();
    // set up connections
}

////////////////////////////////////////////////////////////////////////////////
