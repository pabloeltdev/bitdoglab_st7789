#include "st7789.h"
#include "pins.h"
#include "cmds.h"
#include "error.h"

#include "hardware/spi.h"

#include <stdio.h>
#include <string.h>

struct st7789_t {
    bool is_sleeping;
    bool is_idle;
    bool is_partial;
    bool is_inverted;
};

uint16_t __rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return r << 11 | g  << 5 | b;
}

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

void _init_struct(st7789_t display, bool invert)
{
    display->is_sleeping = false;
    display->is_idle = false;
    display->is_partial = false;
    display->is_inverted = invert;
}

void _frame(uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye) {
        // Corta o conteúdo fora dos limites do display
    const uint16_t LX = ST7789_WIDTH - 1;
    const uint16_t LY = ST7789_HEIGHT - 1;
    size_t width = xe - xs + 1;
    size_t height = ye - ys + 1;
    if (xs < 0) xs = 0;
    if (ys < 0) ys = 0;
    if (xe > LX) xe = LX;
    if (ye > LY) ye = LY;
    if (xs > xe || ys > ye) return;
    
    // Declara os dados de configuração da janela de desenho
    uint8_t data_col[4] = { // bits de coluna
        xs >> 8, xs,
        xe >> 8, xe
    };
    uint8_t data_row[4] = { // bits de linha
        ys >> 8, ys,
        ye >> 8, ye
    };
    // envia janela de desenho
    _st7789_send_command(ST7789_CMD_CASET); 
    _st7789_send_data(data_col, 4);
    _st7789_send_command(ST7789_CMD_RASET);
    _st7789_send_data(data_row, 4);
}

void _fill_empty() 
{
    uint16_t xe = ST7789_WIDTH - 1;
    uint16_t ye = ST7789_HEIGHT - 1;
    size_t total_pixels = ST7789_WIDTH * ST7789_HEIGHT;
    uint8_t data_col[4] = { // bits de coluna
        0x00, 0x00, xe >> 8, xe
    };
    uint8_t data_row[4] = { // bits de linha
        0x00, 0x00, ye >> 8, ye
    };
    
    // envia janela de desenho
    _st7789_send_command(ST7789_CMD_CASET); 
    _st7789_send_data(data_col, 4);
    _st7789_send_command(ST7789_CMD_RASET);
    _st7789_send_data(data_row, 4);
    // inicia a escrita de dados
    _st7789_send_command(ST7789_CMD_RAMWR);
    // Preenche o buffer com a color preta
    uint8_t color_data[2 * total_pixels];
    memset(color_data, 0x00, 2 * total_pixels);
    _st7789_send_data(color_data, 2 * total_pixels);
}

// Inicializa o display
void st7789_init(st7789_t display, bool invert)
{
    // Inicializa a estrutura de estado
    _init_struct(display, invert);
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
    // Define o modo de cor como 16 bits
    _st7789_send_command(ST7789_CMD_COLMOD);
    _st7789_send_byte(0x55);
    // Preenche de preto antes de ligar o display
    _fill_empty();
    // Inverte as cores do display caso solicitado
    if (display->is_inverted) {
        _st7789_send_command(ST7789_CMD_INVON);
    }
    // Liga o display
    _st7789_send_command(ST7789_CMD_DISPON);
}

// Faz um reset de software no display (seção 9.1.2 do datasheet)
void st7789_reset(st7789_t display)
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

void st7789_inversion_on(st7789_t display)
{
    if (display->is_inverted) return;
    _st7789_send_command(ST7789_CMD_INVON);
    display->is_inverted = true;
}

void st7789_inversion_off(st7789_t display)
{
    if (!display->is_inverted) return;
    _st7789_send_command(ST7789_CMD_INVOFF);
    display->is_inverted = false;
}

// Coloca o display em modo de sono (seção 9.1.11 do datasheet)
void st7789_sleep(st7789_t display)
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
void st7789_wake(st7789_t display)
{
    if (!display->is_sleeping) {
        return;
    }
    _st7789_send_command(ST7789_CMD_SLPOUT);
    _st7789_set_backlight(true);
    display->is_sleeping = false;
    busy_wait_ms(120);
}

/**
 * Desenha um retângulo delimitado pelos pontos ({xs}, {ys}) e ({xe}, {ye}) com a cor {color}
 * {xs} - coordenada x inicial
 * {xe} - coordenada x final
 * {ys} - coordenada y inicial
 * {ye} - coordenada y final
 * {color} - cor do retângulo no formato RGB565
 */
void st7789_drawRect(st7789_t display, uint16_t xs, uint16_t xe, uint16_t ys, uint16_t ye, st7789_color_t color)
{
    // Define a janela de desenho
    _frame(xs, xe, ys, ye);
    // Inicia a escrita de dados
    _st7789_send_command(ST7789_CMD_RAMWR);
    // Prepara os dados de cor
    size_t width = xe - xs + 1;
    size_t height = ye - ys + 1;
    size_t total_pixels = width * height;
    uint16_t color565 = __rgb565(color.r, color.g, color.b);
    uint8_t color_data[2 * total_pixels];
    for (size_t i = 0; i < total_pixels; i++) {
        color_data[2 * i] = color565 >> 8;
        color_data[2 * i + 1] = color565;
    }
    // Envia os dados de cor para todos os pixels do retângulo
    _st7789_send_data(color_data, 2 * total_pixels);
}

/**
 * 
 */
void st7789_fill(st7789_t display, st7789_color_t color)
{
    st7789_drawRect(display, 0, ST7789_WIDTH - 1, 0, ST7789_HEIGHT - 1, color);
}

// Desenha um pixel na posição ({x}, {y}) com a cor {color}
void st7789_drawPixel(st7789_t display, uint16_t x, uint16_t y, st7789_color_t color)
{
    st7789_drawRect(display, x, x, y, y, color);
}

void st7789_drawBitmap(st7789_t display, uint16_t xs, uint16_t ys, uint16_t width, uint16_t height, st7789_bitmap_t bitmap)
{
    uint16_t xe = xs + width - 1, ye = height - 1;
    uint32_t total_pixels = width * height;
    // Define a janela de desenho
    _frame(xs, xe, ys, ye);
    // Inicia a escrita de dados
    _st7789_send_command(ST7789_CMD_RAMWR);
    // Envia envia o bitmap completo
    _st7789_send_data((uint8_t*) bitmap, 2 * total_pixels);
}