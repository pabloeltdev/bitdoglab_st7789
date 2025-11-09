#if !defined(ST7789_H)
#define ST7789_H

#include "error.h"

#include "pico/stdlib.h"

#if !defined(FREQ_MHZ)
#define FREQ_MHZ 1000000
#endif // FREQ_MHZ

#define ST7789_SPI_INSTANCE spi0
#define ST7789_SPI_BAUDRATE 40
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 320

typedef struct {
    bool is_sleeping;
    bool is_idle;
    bool is_partial;
} st7789_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} st7789_color_t;

void st7789_init(st7789_t* display);
void st7789_reset(st7789_t* display);
void st7789_sleep(st7789_t* display);
void st7789_wake(st7789_t* display);
void st7789_drawRect(st7789_t* display, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye, st7789_color_t color);
void st7789_fill(st7789_t* display, st7789_color_t color);
void st7789_drawPixel(st7789_t* display, int16_t x, int16_t y, st7789_color_t color);

#endif // ST7789_H
