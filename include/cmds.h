#if !defined(ST7789_CMDS_H)
#define ST7789_CMDS_H

#define ST7789_CMD_NOP        0x00
#define ST7789_CMD_SWRESET    0x01
#define ST7789_CMD_RDDID      0x04
#define ST7789_CMD_RDDST      0x09
#define ST7789_CMD_SLPIN      0x10
#define ST7789_CMD_SLPOUT     0x11
#define ST7789_CMD_PTLON      0x12
#define ST7789_CMD_NORON      0x13
#define ST7789_CMD_INVOFF     0x20
#define ST7789_CMD_INVON      0x21
#define ST7789_CMD_DISPOFF    0x28
#define ST7789_CMD_DISPON     0x29
#define ST7789_CMD_CASET      0x2A
#define ST7789_CMD_RASET      0x2B
#define ST7789_CMD_RAMWR      0x2C
#define ST7789_CMD_MADCTL     0x36
#define ST7789_CMD_COLMOD     0x3A

#endif // ST7789_CMDS_H
