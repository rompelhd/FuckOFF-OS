#include "print.h"

void kernel_main() {
    print_clear();
    idt_init();

    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome to ");

    print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
    print_str("FuckOFF kernel ");

    print_set_color(PRINT_COLOR_BLUE, PRINT_COLOR_BLACK);
    print_str("v.0.1 ");

    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("by ");

    print_set_color(PRINT_COLOR_RED, PRINT_COLOR_BLACK);
    print_str("Rompelhd!\n");

    // Test: (IRQ0)
    asm volatile ("int $32");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
