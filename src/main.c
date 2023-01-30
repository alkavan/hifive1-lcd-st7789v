#include <metal/cpu.h>
#include <metal/pwm.h>
#include <metal/io.h>
#include <metal/machine.h>
#include <metal/uart.h>
#include <metal/spi.h>
#include <metal/gpio.h>

#include <stdio.h>
#include <time.h>

#include "fonts.h"

#define RET_OK 0
#define RET_ERR 1

#define RTC_FREQ 32768
#define CLOCK_FREQ 32.768

#define MIN_DUTY 0
#define MAX_DUTY 90

#define PWM_FREQ 1000
#define PWM_DEVICE_NUM 1

#define SPI_DEVICE_NUM 1
#define SPI_BAUD 100000

#define GPIO_DEVICE_NUM 0

#define LCD_GPIO_CS  16
#define LCD_GPIO_DC  17
#define LCD_GPIO_RST 18

#define LCD_WIDTH  240 // LCD width in pixels
#define LCD_HEIGHT 320 // LCD height in pixels

#define COLOR_WHITE     0xFFFF
#define COLOR_GRAY      0X8430
#define COLOR_BLACK     0x0000

#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0

#define COLOR_MAGENTA   0xF81F
#define COLOR_CYAN      0x7FFF

#define COLOR_DARKBLUE  0X01CF
#define COLOR_LIGHTBLUE 0X7D7C

#define FONT_BACKGROUND 0X01CF

struct metal_pwm *pwm;
struct metal_spi *spi;
struct metal_gpio *gpio;

struct metal_spi_config *spi_config;

void sleep_ticks(unsigned long ticks) {
    clock_t stop = clock() + ticks;
    while (stop > clock());
}

void sleep_ms(unsigned long ms) {
    sleep_ticks(ms*1000);
}

void sleep(unsigned long seconds) {
    unsigned long stop = time(NULL) + seconds;
    while (stop > time(NULL));
}

void lcd_reset() {
    metal_gpio_set_pin(gpio, LCD_GPIO_RST, METAL_GPIO_INT_HIGH);
    sleep_ms(100);
    metal_gpio_set_pin(gpio, LCD_GPIO_RST, METAL_GPIO_INT_DISABLE);
    sleep_ms(100);
    metal_gpio_set_pin(gpio, LCD_GPIO_RST, METAL_GPIO_INT_HIGH);
    sleep_ms(100);
}

void lcd_write_command(uint8_t data) {
    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_DISABLE);
    metal_gpio_set_pin(gpio, LCD_GPIO_DC, METAL_GPIO_INT_DISABLE);

    char tx_buf[1] = {data};
    metal_spi_transfer(spi, spi_config, 1, tx_buf, NULL);

    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_HIGH);
}

void lcd_write_byte(uint8_t data) {
    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_DISABLE);
    metal_gpio_set_pin(gpio, LCD_GPIO_DC, METAL_GPIO_INT_HIGH);

    char tx_buf[1] = {data};
    metal_spi_transfer(spi, spi_config, 1, tx_buf, NULL);

    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_HIGH);
}

void lcd_write_word(uint16_t data) {
    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_DISABLE);
    metal_gpio_set_pin(gpio, LCD_GPIO_DC, METAL_GPIO_INT_HIGH);

    char tx_buf[2] = {(data>>8) & 0xff, data};
    metal_spi_transfer(spi, spi_config, 2, tx_buf, NULL);

    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_HIGH);
}

void lcd_write_data(char *data, size_t size) {
    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_DISABLE);
    metal_gpio_set_pin(gpio, LCD_GPIO_DC, METAL_GPIO_INT_HIGH);

    metal_spi_transfer(spi, spi_config, size, data, NULL);

    metal_gpio_set_pin(gpio, LCD_GPIO_CS, METAL_GPIO_INT_HIGH);
}

void lcd_init() {
    lcd_reset();
    
    lcd_write_command(0x36);
    lcd_write_byte(0x00);

    lcd_write_command(0x3A);
    lcd_write_byte(0x05);

    lcd_write_command(0x21);

    lcd_write_command(0x2A);
    lcd_write_byte(0x00);
    lcd_write_byte(0x00);
    lcd_write_byte(0x01);
    lcd_write_byte(0x3F);

    lcd_write_command(0x2B);
    lcd_write_byte(0x00);
    lcd_write_byte(0x00);
    lcd_write_byte(0x00);
    lcd_write_byte(0xEF);

    lcd_write_command(0xB2);
    lcd_write_byte(0x0C);
    lcd_write_byte(0x0C);
    lcd_write_byte(0x00);
    lcd_write_byte(0x33);
    lcd_write_byte(0x33);

    lcd_write_command(0xB7);
    lcd_write_byte(0x35);

    lcd_write_command(0xBB);
    lcd_write_byte(0x1F);

    lcd_write_command(0xC0);
    lcd_write_byte(0x2C);

    lcd_write_command(0xC2);
    lcd_write_byte(0x01);

    lcd_write_command(0xC3);
    lcd_write_byte(0x12);

    lcd_write_command(0xC4);
    lcd_write_byte(0x20);

    lcd_write_command(0xC6);
    lcd_write_byte(0x0F);

    lcd_write_command(0xD0);
    lcd_write_byte(0xA4);
    lcd_write_byte(0xA1);

    lcd_write_command(0xE0);
    lcd_write_byte(0xD0);
    lcd_write_byte(0x08);
    lcd_write_byte(0x11);
    lcd_write_byte(0x08);
    lcd_write_byte(0x0C);
    lcd_write_byte(0x15);
    lcd_write_byte(0x39);
    lcd_write_byte(0x33);
    lcd_write_byte(0x50);
    lcd_write_byte(0x36);
    lcd_write_byte(0x13);
    lcd_write_byte(0x14);
    lcd_write_byte(0x29);
    lcd_write_byte(0x2D);

    lcd_write_command(0xE1);
    lcd_write_byte(0xD0);
    lcd_write_byte(0x08);
    lcd_write_byte(0x10);
    lcd_write_byte(0x08);
    lcd_write_byte(0x06);
    lcd_write_byte(0x06);
    lcd_write_byte(0x39);
    lcd_write_byte(0x44);
    lcd_write_byte(0x51);
    lcd_write_byte(0x0B);
    lcd_write_byte(0x16);
    lcd_write_byte(0x14);
    lcd_write_byte(0x2F);
    lcd_write_byte(0x31);
    lcd_write_command(0x21);

    lcd_write_command(0x11);

    lcd_write_command(0x29);
}

void lcd_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    lcd_write_command(0x2a);
    lcd_write_byte(x0 >> 8);
    lcd_write_byte(x0 & 0xff);
    lcd_write_byte((x1 - 1) >> 8);
    lcd_write_byte((x1 - 1) & 0xff);

    lcd_write_command(0x2b);
    lcd_write_byte(y0 >> 8);
    lcd_write_byte(y0 & 0xff);
    lcd_write_byte((y1 - 1) >> 8);
    lcd_write_byte((y1 - 1) & 0xff);

    lcd_write_command(0x2C);
}

void lcd_set_cursor(uint16_t x, uint16_t y)
{
    lcd_write_command(0x2a);
    lcd_write_byte(x >> 8);
    lcd_write_byte(x);
    lcd_write_byte(x >> 8);
    lcd_write_byte(x);

    lcd_write_command(0x2b);
    lcd_write_byte(y >> 8);
    lcd_write_byte(y);
    lcd_write_byte(y >> 8);
    lcd_write_byte(y);

    lcd_write_command(0x2C);
}

void lcd_clear(uint16_t color)
{
    uint16_t i, j;
    lcd_set_window(0, 0, LCD_WIDTH, LCD_HEIGHT);

    metal_gpio_set_pin(gpio, LCD_GPIO_DC, METAL_GPIO_INT_HIGH);

    for(i = 0; i < LCD_WIDTH; i++) {
        uint16_t line[LCD_HEIGHT];

        for(j = 0; j < LCD_HEIGHT; j++) {
            line[j] = color;
            lcd_write_word(color);
        }
//        lcd_write_screen_line(line, LCD_HEIGHT);
    }
}

void lcd_clear_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    uint16_t i, j;
    lcd_set_window(x0, y0, x1, y1);
    for(i = x0; i <= y1; i++){
        for(j = y0; j <= x1; j++){
            lcd_write_word(color);
        }
    }
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_cursor(x, y);
    lcd_write_word(color);
}

void lcd_draw_char(uint16_t x, uint16_t y, const char character, FontType *font, uint16_t bg, uint16_t fg)
{
    uint16_t page, column;

    if (x > LCD_WIDTH || y > LCD_HEIGHT) {
        printf("lcs_draw_char(): input exceeds the normal display range\n");
        return;
    }

    uint32_t Char_Offset = (character - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[Char_Offset];

    for (page = 0; page < font->Height; page ++ ) {
        for (column = 0; column < font->Width; column ++ ) {

            // to determine whether the font background color and screen background color is consistent
            if (FONT_BACKGROUND == bg) { // this process is to speed up the scan
                if (*ptr & (0x80 >> (column % 8)))
                    lcd_draw_pixel(x + column, y + page, fg);
            } else {
                if (*ptr & (0x80 >> (column % 8))) {
                    lcd_draw_pixel(x + column, y + page, fg);
                } else {
                    lcd_draw_pixel(x + column, y + page, bg);
                }
            }

            // one pixel is 8 bits
            if (column % 8 == 7)
                ptr++;
        } // write a line
        if (font->Width % 8 != 0)
            ptr++;
    } // write all
}

void lcd_draw_string(uint16_t x0, uint16_t y0, const char *text, FontType *Font, uint16_t bg, uint16_t fg) {
    uint16_t x = x0;
    uint16_t y = y0;

    if (x0 > LCD_WIDTH || y0 > LCD_HEIGHT) {
        printf("lcd_draw_string(): input exceeds the normal display range\n");
        return;
    }

    while (*text != '\0') {
        // if X direction filled, reposition to(x, y), y is Y direction plus the Height of the character
        if ((x + Font->Width) > LCD_WIDTH) {
            x = x0;
            y += Font->Height;
        }

        // if the Y direction is full, reposition to(x, y)
        if ((y + Font->Height) > LCD_HEIGHT) {
            x = x0;
            y = y0;
        }

        lcd_draw_char(x, y, *text, Font, bg, fg);

        // the next character of the address
        text++;

        // the next word of the abscissa increases the font of the broadband
        x += Font->Width;
    }
}

int main() {
    printf("Initializing Firmware @ %s %s\n", __DATE__, __TIME__);

//// PWM ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // get a PWM device instance
    pwm = metal_pwm_get_device(PWM_DEVICE_NUM);

    if (pwm == NULL) {
        printf("error: PWM not available\n");
        return RET_ERR;
    }

    // enable PWM instance
    metal_pwm_enable(pwm);

    // set PWM1 to desired frequency
    metal_pwm_set_freq(pwm, 0, PWM_FREQ);

    // set duty cycle and phase correct mode
    metal_pwm_set_duty(pwm, 1, MAX_DUTY, METAL_PWM_PHASE_CORRECT_DISABLE);

    // start in continuous mode
    metal_pwm_trigger(pwm, 0, METAL_PWM_CONTINUOUS);
//
//    int direction = 1;
//    unsigned int duty = MAX_DUTY;
//
//    // main loop
//    while (1) {
//        if(duty == MAX_DUTY) direction = -1;
//        if(duty == MIN_DUTY) direction = 1;
//        duty += direction;
//
//        metal_pwm_set_duty(pwm, 1, duty, METAL_PWM_PHASE_CORRECT_DISABLE);
//
//        uint64_t now = time(NULL);
//        uint64_t tick = clock();
//
//        printf("clock: %lu, time: %lu, duty: %u\n", (unsigned long)tick, (unsigned long)now, duty);
//        sleep_ms(10);
//    }

//// GPIO //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gpio = metal_gpio_get_device(GPIO_DEVICE_NUM);

    if (gpio == NULL) {
        printf("error: GPIO not available\n");
        return RET_ERR;
    }

    metal_gpio_enable_output(gpio, LCD_GPIO_CS);
    metal_gpio_disable_pinmux(gpio, LCD_GPIO_CS);

    metal_gpio_enable_output(gpio, LCD_GPIO_DC);
    metal_gpio_disable_pinmux(gpio, LCD_GPIO_DC);

    metal_gpio_enable_output(gpio, LCD_GPIO_RST);
    metal_gpio_disable_pinmux(gpio, LCD_GPIO_RST);

    metal_gpio_clear_pin(gpio, LCD_GPIO_CS);
    metal_gpio_clear_pin(gpio, LCD_GPIO_DC);
    metal_gpio_clear_pin(gpio, LCD_GPIO_RST);

//// SPI ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // get SPI device
    spi = metal_spi_get_device(SPI_DEVICE_NUM);

    // fallback to SPI device 0
    if(spi == NULL) {
        spi = metal_spi_get_device(0);
    }

    if(spi == NULL) {
        printf("error: SPI not available\n");
        return RET_ERR;
    }

    // initialize the SPI device to desired baud
    metal_spi_init(spi, SPI_BAUD);

    /* CPOL = 0, CPHA = 0, MSB-first, CS active low */
    struct metal_spi_config config = {
            .protocol = METAL_SPI_SINGLE,
            .polarity = 0,
            .phase = 0,
            .little_endian = 0,
            .cs_active_high = 0,
            .csid = 0,
    };

    spi_config = &config;

    lcd_init();
    lcd_clear(COLOR_DARKBLUE);
    lcd_draw_string(20, 100, "MESS WITH THE BEST,", &Font16, COLOR_DARKBLUE,COLOR_WHITE);
    lcd_draw_string(20, 120, "DIE LIKE THE REST.", &Font16, COLOR_DARKBLUE,COLOR_WHITE);
//    lcd_clear_window(100, 100, 200, 200, COLOR_WHITE);

    return RET_OK;
}
