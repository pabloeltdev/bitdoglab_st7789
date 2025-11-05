#if !defined(ST7789_H)
#define ST7789_H

#define ST7789_SPI_INSTANCE spi0
#define ST7789_SPI_BAUDRATE 4
#define FREQ_MHZ 1000000

void st7789_init();
void st7789_drawPixel(int16_t x, int16_t y, uint16_t color);
void st7789_fillScreen(uint16_t color);

#endif // ST7789_H
