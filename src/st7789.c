#include "st7789.h"
#include "pins.h"
#include "cmds.h"
#include "error.h"

#include "hardware/spi.h"

#include <stdio.h>

#define RGB565(r,g,b) (~(((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F)) & 0xFFFF)

// Liga a luz de fundo do display
void _st7789_set_backlight(bool is_on)
{
    gpio_put(ST7789_BLK_PIN, is_on);
}

// Coloca o display em modo comando
void _st7789_command_mode(void)
{
    gpio_put(ST7789_DC_PIN, 0);
    busy_wait_us(1);
}

// Coloca o display em modo dados
void _st7789_data_mode(void)
{
    gpio_put(ST7789_DC_PIN, 1);
    busy_wait_us(1);
}

// Seleciona o display (CS em nível baixo)
void _st7789_select(void)
{
    gpio_put(ST7789_CS_PIN, 0);
    busy_wait_us(1);
}

// Deseleciona o display (CS em nível alto)
void _st7789_deselect(void)
{
    gpio_put(ST7789_CS_PIN, 1);
    busy_wait_us(1);
}

void _st7789_reset()
{
    gpio_put(ST7789_RES_PIN, 0);
    busy_wait_us(10);
    gpio_put(ST7789_RES_PIN, 1);
    busy_wait_us(1);
}

// Envia um comando para o display
void _st7789_send_command(uint8_t cmd)
{
    _st7789_select();
    _st7789_command_mode();
    spi_write_blocking(ST7789_SPI_INSTANCE, &cmd, 1);
    _st7789_deselect();
}

void _st7789_send_data(uint8_t* data, size_t length)
{
    _st7789_select();
    _st7789_data_mode();
    spi_write_blocking(ST7789_SPI_INSTANCE, data, length);
    _st7789_deselect();
}

void _st7789_send_byte(uint8_t byte)
{
   _st7789_send_data(&byte, 1);
}

void _spi_init_pins()
{
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
    // Inicia sem o display selecionado
    gpio_put(ST7789_CS_PIN, 1);
}

void _init_struct(st7789_t* display)
{
    display->is_sleeping = false;
    display->is_idle = false;
    display->is_partial = false;
}

// Inicializa o display
void st7789_init(st7789_t* display)
{
    // Inicializa a estrutura de estado
    _init_struct(display);
    // Inicializa a SPI
    spi_init(ST7789_SPI_INSTANCE, ST7789_SPI_BAUDRATE * FREQ_MHZ);
    // Define os pinos de função SPI 
    gpio_set_function(ST7789_SCL_PIN, GPIO_FUNC_SPI);
    gpio_set_function(ST7789_SDA_PIN, GPIO_FUNC_SPI);
    // Configura os resistores de pull-up do SPI
    gpio_pull_up(ST7789_SCL_PIN);
    gpio_pull_up(ST7789_SDA_PIN);
    // Inicializa os pinos de controle
    _spi_init_pins();
    // Reseta o display
    _st7789_reset();
    // Liga o backlight
    _st7789_set_backlight(true);
    // Saída do modo de sono
    _st7789_send_command(ST7789_CMD_SLPOUT);
    // Modo de cor: 16-bit
    _st7789_send_command(ST7789_CMD_COLMOD);
    _st7789_send_byte(0x55);
    // Liga o display
    _st7789_send_command(ST7789_CMD_DISPON);
}

// Faz um reset de software no display (seção 9.1.2 do datasheet)
void st7789_reset(st7789_t* display)
{
    // Envia o comando de reset de software
    _st7789_send_command(ST7789_CMD_SWRESET);
    if (display->is_sleeping) {
        // Reseta o tempo de espera para sair do modo de sono ()
        busy_wait_ms(120);
    } else {
        // Espera o tempo padrão de reset
        busy_wait_ms(5);
    }
}

// Coloca o display em modo de sono (seção 9.1.11 do datasheet)
void st7789_sleep(st7789_t* display)
{
    if (display->is_sleeping) {
        return;
    }
    _st7789_send_command(ST7789_CMD_SLPIN);
    _st7789_set_backlight(false);
    display->is_sleeping = true;
    busy_wait_ms(5);
}

// Desperta o display do modo de sono (seção 9.1.12 do datasheet)
void st7789_wake(st7789_t* display)
{
    if (!display->is_sleeping) {
        return;
    }
    _st7789_send_command(ST7789_CMD_SLPOUT);
    _st7789_set_backlight(true);
    display->is_sleeping = false;
    busy_wait_ms(120);
}

// Desenha um pixel na posição ({x}, {y}) com a cor {color}
void st7789_drawPixel(st7789_t* display, int16_t x, int16_t y, uint16_t color)
{
}