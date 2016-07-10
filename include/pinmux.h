#ifndef PINMUX_H_CDYTITMB
#define PINMUX_H_CDYTITMB

#include <eagle_soc.h>

// map GPIO pin numbers to IO_MUX name
// this is not fully tested, beware of typos!
#define PERIPH_IO_MUX__0        PERIPHS_IO_MUX_GPIO0_U
#define PERIPH_IO_MUX__1        PERIPHS_IO_MUX_U0TXD_U
#define PERIPH_IO_MUX__2        PERIPHS_IO_MUX_GPIO2_U
#define PERIPH_IO_MUX__3        PERIPHS_IO_MUX_U0RXD_U
#define PERIPH_IO_MUX__4        PERIPHS_IO_MUX_GPIO4_U
#define PERIPH_IO_MUX__5        PERIPHS_IO_MUX_GPIO5_U
#define PERIPH_IO_MUX__6        PERIPHS_IO_MUX_SD_CLK_U
#define PERIPH_IO_MUX__7        PERIPHS_IO_MUX_SD_DATA_0_U
#define PERIPH_IO_MUX__8        PERIPHS_IO_MUX_SD_DATA_1_U
#define PERIPH_IO_MUX__9        PERIPHS_IO_MUX_SD_DATA_2_U
#define PERIPH_IO_MUX__10       PERIPHS_IO_MUX_SD_DATA_3_U
#define PERIPH_IO_MUX__11       PERIPHS_IO_MUX_SD_CMD_U
#define PERIPH_IO_MUX__12       PERIPHS_IO_MUX_MTDI_U
#define PERIPH_IO_MUX__13       PERIPHS_IO_MUX_MTCK_U
#define PERIPH_IO_MUX__14       PERIPHS_IO_MUX_MTMS_U
#define PERIPH_IO_MUX__15       PERIPHS_IO_MUX_MTDO_U


#define __CAT(x, y) x ## y
#define _CAT(x, y) __CAT(x, y)
#define CAT(x, y) _CAT(x, y)

#define PIN_BIT(pinnr)          CAT(BIT, pinnr)
#define PIN_MUX(pinnr)          CAT(PERIPH_IO_MUX__, pinnr)
#define PIN_FUNC(pinnr)         CAT(FUNC_GPIO, pinnr)

#endif /* end of include guard: PINMUX_H_CDYTITMB */
