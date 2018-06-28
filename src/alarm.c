#include "alarm.h"

#define PROMPT "> "

////////////////////////////////////////////////////////////////////////////////
// actual alarm stuff

int timezone_offset = 2; // MESZ ;)
time_t current_alarm_time = 0;
int repeat_alarm = 0; // number of times to repeat alarm (-1 = inf)

unsigned int anim_duration = 30 * 60; // animate over half an hour

////////////////////////////////////////////////////////////////////////////////
// SERVER
////////////////////////////////////////////////////////////////////////////////

// TODO: use gcc intrinsics for single-eval implementation
#define MIN(a, b)   ((a)<(b)?(a):(b))

////////////////////////////////////////////////////////////////////////////////

struct espconn alarm_server_con;
esp_tcp alarm_server_con_tcp;

void ICACHE_FLASH_ATTR alarm_server_rx(void * arg, char* data, unsigned short len)
{
    struct espconn * conn = (struct espconn *) arg;

    char inputstr[256];
    char outputbuf[256] = "";
    trimwhitespace(inputstr, sizeof(inputstr), data);

    #define IS_CMD(str) (strncmp(inputstr, (str), MIN(strlen((str)), len)) == 0)
    #define IS_CHAR_CMD(c)  (len == 3 && inputstr[0] == (c) && inputstr[1] == '\0')

    #define __RESPONSE(fmt, ...) do { \
        os_sprintf(outputbuf, "%s" fmt "\n", outputbuf, __VA_ARGS__); \
    } while(0)

    #define RESPONSE(...)  __RESPONSE(__VA_ARGS__, 0)


    if (IS_CMD("help")) {
        RESPONSE(
                "led $r $g $b $w1 $w2   - set LED fix color\n" // TODO
                "status             - show time and currently set alarm\n"
                "set_tz $offset     - set timezone offset relative to UTC\n"
                "set_alarm $h $m    - arm alarm\n"
                "repeat $n          - number of repetitions (0=none, -1=inf)\n"
                "clr_alarm          - disable alarm\n"
                "ntp                - query NTP server to update time\n"
                "quit               - close this TCP session\n"
                "help               - print this information ;)");
    } else if (IS_CMD("set_tz")) {
        // TODO: implement error handling correctly
        errno = 0; char * tmp;
        int offset = strtol(&inputstr[6], &tmp, 0);
        if (errno == 0 && *tmp == '\0') {
            timezone_offset = offset;
            RESPONSE("timezone offset set to %d", timezone_offset);
        } else {
            RESPONSE("ERROR: invalid timezone offset (errno: %d)", errno);
        }

    } else if (IS_CMD("status") || IS_CHAR_CMD('s')) {
        time_t * tptr = getLocalTime();
        if (tptr == NULL) {
            RESPONSE("NTP not running, time unknown.");
        } else {

            time_t t = *tptr;

            t += timezone_offset * 60 * 60;

            tmElements_t tm_now;
            simple_localtime(t, &tm_now);

            RESPONSE("current time: %d:%02d.%02d (UTC+%d)",
                    tm_now.Hour, tm_now.Minute, tm_now.Second, timezone_offset);

            if (current_alarm_time == 0) {
                RESPONSE("no alarm set");
            } else {
                tmElements_t tm_alarm;
                simple_localtime(current_alarm_time, &tm_alarm);
                RESPONSE("alarm set:    %d:%02d.%02d", tm_alarm.Hour, tm_alarm.Minute, tm_alarm.Second);

                // TODO: this calculation is wrong
                t = tm_now.Hour * 60 * 60 + tm_now.Minute * 60 + tm_now.Second;
                t = current_alarm_time - t;

                simple_localtime(t, &tm_alarm);
                RESPONSE("this is in: %d:%02d.%02d", tm_alarm.Hour, tm_alarm.Minute, tm_alarm.Second);

                if (repeat_alarm == 0) {
                    RESPONSE("alarm will NOT be repeated");
                } else if (repeat_alarm > 0) {
                    RESPONSE("alarm will be repeated %d times", repeat_alarm);
                } else {
                    RESPONSE("alarm will be repeated indefinitely");
                }
            }
        }

    } else if (IS_CMD("set_alarm")) {
        char * tmp;
        int hours   = strtol(&inputstr[10], &tmp, 0);
        int minutes = strtol(tmp, NULL, 0);

        if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
            RESPONSE("Invalid time: %d : %d", hours, minutes);
        } else {
            current_alarm_time = hours * 60 * 60 + minutes * 60;
            tmElements_t tm;
            simple_localtime(current_alarm_time, &tm);
            RESPONSE("alarm set to: %d:%02d.%02d", tm.Hour, tm.Minute, tm.Second);
        }

    } else if (IS_CMD("repeat")) {
        char * tmp;
        repeat_alarm = strtol(&inputstr[6], &tmp, 0);
        RESPONSE("alarm repetition count set to: %ld", repeat_alarm);

    } else if (IS_CMD("clr_alarm")) {
        current_alarm_time = 0;
        stop_current_animation();
        RESPONSE("alarm disarmed");

    } else if (IS_CMD("led")) {

        uint8_t rgbww[5] = {0,0,0, 0,0};

        // parse max 5 color parameters
        char * rd_ptr = &inputstr[3];
        size_t i = 0;
        for (; i < sizeof(rgbww); i++) {
            char * next_ptr;
            uint8_t v = strtol(rd_ptr, &next_ptr, 0);
            if (next_ptr == rd_ptr) {
                // no chars parsed
                break;
            }
            rd_ptr = next_ptr;

            rgbww[i] = v;
        }
        LED_set_arr(rgbww);
        RESPONSE("new color: %02x:%02x:%02x:%02x:%02x", rgbww[0], rgbww[1], rgbww[2], rgbww[3],rgbww[4]);

    } else if (IS_CMD("ntp")) {
        sendNTPpacket();
    } else if (IS_CMD("quit")) {
        espconn_disconnect(conn);
    } else {
        RESPONSE("ERROR: Unkown command. See 'help' for a list of valid commands.");
    }

    // append PROMPT without trailing newline
    os_sprintf(outputbuf, "%s" PROMPT, outputbuf);
    espconn_send(conn, outputbuf, strlen(outputbuf));
}

/* not really required
void ICACHE_FLASH_ATTR server_connection_closed_cb(void * arg)
{
    os_printf("connection closed\n");
}
*/

void ICACHE_FLASH_ATTR alarm_server_accept_connect_cb(void * arg)
{
    struct espconn * conn = (struct espconn *) arg;

    os_printf("got incoming connection from " IPSTR "\n", IP2STR(conn->proto.tcp->remote_ip));

    //espconn_regist_recvcb(conn, server_rx_data_cb);
    //espconn_regist_disconcb(conn, server_connection_closed_cb);

    char greeting[] = "[[[ LIGHT ALARM CLOCK SERVER ]]]\n\n"
        "Welcome to the WiFiLi ACS. Enter 'help' to list all available commands.\n\n"
        PROMPT;

    espconn_sent(conn, greeting, sizeof(greeting));
}

////////////////////////////////////////////////////////////////////////////////

static os_timer_t alarm_timer;

void alarm_timer_func(void *arg)
{
    time_t* tptr = getLocalTime();
    if (tptr && current_alarm_time != 0) {
        time_t t = *tptr;
        t += timezone_offset * 60 * 60;
        // we have a valid localtime and alarm is armed
        tmElements_t tm_now;
        simple_localtime(t, &tm_now);

        tmElements_t tm_alarm;
        simple_localtime(current_alarm_time, &tm_alarm);

        if (tm_now.Hour == tm_alarm.Hour && tm_now.Minute == tm_alarm.Minute) {
            // alarm goes off!
            start_animation(mkanim_sunset(anim_duration), 1);

            if (repeat_alarm == 0) {
                // don't repeat
                current_alarm_time = 0; // disable alarm
            } else if (repeat_alarm > 0) {
                --repeat_alarm;
            } // else (<0): repeat indefinitely

            os_printf("[ALARM] ALARM ALARM ALARM!!!\n");
        } else {
            os_printf("[ALARM] not yet time for alarm...\n");
            os_printf("[ALARM] alarm at %d:%02d, now is %d:%02d\n", tm_alarm.Hour, tm_alarm.Minute, tm_now.Hour, tm_now.Minute);
        }
    } else {
        //os_printf("[ALARM] no alarm set or invalid localtime\n");
    }
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR alarm_server_init(unsigned int port)
{
    // set up server
    espconn_create(&alarm_server_con);
    alarm_server_con.type  = ESPCONN_TCP;
    alarm_server_con.state = ESPCONN_NONE;
    alarm_server_con.proto.tcp = &alarm_server_con_tcp;
    alarm_server_con.proto.tcp->local_port = port;

    // set callback for incoming connections
    espconn_regist_connectcb(&alarm_server_con, alarm_server_accept_connect_cb);

    // can we set the callbacks already here?
    espconn_regist_recvcb(&alarm_server_con, alarm_server_rx);
    //espconn_regist_disconcb(&alarm_server_con, server_connection_closed_cb);

    // start listening
    espconn_accept(&alarm_server_con);

    // set TCP timeout
    espconn_regist_time(&alarm_server_con, 1000, 0);

    //Setup timer
    os_timer_setfn(&alarm_timer, (os_timer_func_t *)alarm_timer_func, NULL);

    //Arm the timer
    os_timer_arm(&alarm_timer, 60*500, 1); // check every half minute if alarm should go off
}

////////////////////////////////////////////////////////////////////////////////
