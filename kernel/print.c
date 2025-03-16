#include "print.h"
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
uint8_t text_color = 0x0F; // White on black

void print_clear() {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)(text_color << 8);
        }
    }
}

void print_set_color(uint8_t foreground, uint8_t background) {
    text_color = (background << 4) | (foreground & 0x0F);
}

void print_char(char character) {
    static size_t index = 0;
    if (character == '\n') {
        index += VGA_WIDTH - (index % VGA_WIDTH);
    } else {
        vga_buffer[index] = (uint16_t)character | (uint16_t)(text_color << 8);
        index++;
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

    char buffer[10];
    int i = 0;

    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    // Reverse buffer
    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}
