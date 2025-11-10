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
struct st7789_t;
typedef struct st7789_t *st7789_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} st7789_color_t;

typedef const uint8_t st7789_bitmap_t[];

void st7789_init(st7789_t display, bool invert);
void st7789_reset(st7789_t display);
void st7789_inversion_on(st7789_t display);
void st7789_inversion_off(st7789_t display);
void st7789_sleep(st7789_t display);
void st7789_wake(st7789_t display);
void st7789_drawRect(st7789_t display, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye, st7789_color_t color);
void st7789_fill(st7789_t display, st7789_color_t color);
void st7789_drawPixel(st7789_t display, uint16_t x, uint16_t y, st7789_color_t color);
void st7789_drawBitmap(st7789_t display, uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, st7789_bitmap_t bitmap);

#endif // ST7789_H
