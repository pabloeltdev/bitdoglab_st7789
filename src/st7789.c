#include "st7789.h"
#include "pins.h"

#include "hardware/spi.h"

void _spi_init_pins() {
    // Inicializa os pinos de controle
    gpio_init(ST7789_RES_PIN);
    gpio_init(ST7789_DC_PIN);
    gpio_init(ST7789_CS_PIN);
    gpio_init(ST7789_BLK_PIN);
    // Configura os pinos de saída
    gpio_set_dir(ST7789_RES_PIN, GPIO_OUT);
    gpio_set_dir(ST7789_DC_PIN, GPIO_OUT);
    gpio_set_dir(ST7789_CS_PIN, GPIO_OUT);
    gpio_set_dir(ST7789_BLK_PIN, GPIO_OUT);
    // Configura os resistores de pull-up
    gpio_pull_up(ST7789_SCL_PIN);
    gpio_pull_up(ST7789_SDA_PIN);
    gpio_pull_up(ST7789_RES_PIN);
    gpio_pull_up(ST7789_CS_PIN);
    // Configura os resistores de pull-down
    gpio_pull_down(ST7789_BLK_PIN);
    gpio_pull_down(ST7789_DC_PIN);
    // NOTE: Como o DC não está sendo utilizado, ele deve
    // ficar em GND ou VCC, segundo o datasheet.
}

// Inicializa o display
void st7789_init()
{
    // Inicializa a SPI
    spi_init(SPI_INSTANCE, SPI_BAUDRATE * FREQ_MHZ);
    // Define os pinos de função SPI 
    gpio_set_function(ST7789_SCL_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ST7789_SDA_PIN, GPIO_FUNC_SPI);
    // Inicializa os demais pinos
    _spi_init_pins();

}

// Desenha um pixel na posição ({x}, {y}) com a cor {color}
void st7789_drawPixel(int16_t x, int16_t y, uint16_t color)
{
}

// Preenche a tela com a cor {color}
void st7789_fillScreen(uint16_t color)
{
}
