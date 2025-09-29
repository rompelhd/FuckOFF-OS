#include "print.h"
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_SIZE (VGA_WIDTH * VGA_HEIGHT)

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
uint8_t text_color = 0x0F; // White on black
size_t cursor_x = 0;
size_t cursor_y = 0;

void print_scroll() {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)(text_color << 8);
    }
    cursor_y = VGA_HEIGHT - 1;
}

void print_clear() {
    for (size_t i = 0; i < VGA_SIZE; i++) {
        vga_buffer[i] = (uint16_t)' ' | (uint16_t)(text_color << 8);
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_set_color(uint8_t foreground, uint8_t background) {
    text_color = (background << 4) | (foreground & 0x0F);
}

void print_at(char character, size_t x, size_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    vga_buffer[y * VGA_WIDTH + x] = (uint16_t)character | (uint16_t)(text_color << 8);
}

void print_char(char character) {
    if (character == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (character == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            print_at(' ', cursor_x, cursor_y);
        }
    } else {
        print_at(character, cursor_x, cursor_y);
        cursor_x++;
    }
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= VGA_HEIGHT) {
        print_scroll();
    }
}

void print_str(const char* string) {
    for (size_t i = 0; string[i] != '\0'; i++) {
        print_char(string[i]);
    }
}

void print_int(int num) {
    if (num == 0) {
        print_char('0');
        return;
    }
    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    char buffer[11];
    int i = 0;
    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    if (is_negative) {
        buffer[i++] = '-';
    }
    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}

void print_hex(uint32_t num) {
    const char* hex_digits = "0123456789ABCDEF";
    if (num == 0) {
        print_char('0');
        return;
    }
    char buffer[9];
    int i = 0;
    while (num > 0) {
        buffer[i++] = hex_digits[num % 16];
        num /= 16;
    }
    print_str("0x");
    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}

// Format string with %s, %d, %x support
void print_fmt(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's':
                    print_str(va_arg(args, const char*));
                    break;
                case 'd':
                    print_int(va_arg(args, int));
                    break;
                case 'x':
                    print_hex(va_arg(args, uint32_t));
                    break;
                default:
                    print_char(fmt[i]);
                    break;
            }
        } else {
            print_char(fmt[i]);
        }
    }
    va_end(args);
}