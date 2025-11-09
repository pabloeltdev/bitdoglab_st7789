#if !defined(ST7789_H)
#define ST7789_H

#include "error.h"

#include "pico/stdlib.h"

#define FREQ_MHZ 1000000

#define ST7789_SPI_INSTANCE spi0
#define ST7789_SPI_BAUDRATE 40
#define ST7789_WIDTH 240
#define ST7789_HEIGHT 320

typedef struct {
    bool is_sleeping;
    bool is_idle;
    bool is_partial;
} st7789_t;


void st7789_init(st7789_t* display);
void st7789_drawPixel(st7789_t* display, int16_t x, int16_t y, uint16_t color);
void st7789_fillScreen(st7789_t* display, uint16_t color);
void st7789_reset(st7789_t* display);
void st7789_sleep(st7789_t* display);
void st7789_wake(st7789_t* display);



#endif // ST7789_H
