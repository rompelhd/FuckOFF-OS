#include "isr.h"
#include "print.h"

void isr_handler(int interrupt_number) {
    print_str("Interruption received: ");
    print_char('0' + interrupt_number);
    print_str("\n");
}

void isr_timer() {
    print_str("Timer Interrupt!\n");
}

void isr_keyboard() {
    print_str("Keyboard Interrupt!\n");
}

void (*interrupt_handlers[256])(void);

void register_interrupt_handler(int n, void (*handler)(void)) {
    interrupt_handlers[n] = handler;
}
