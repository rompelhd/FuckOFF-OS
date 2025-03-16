#include "timer.h"
#include "isr.h"
#include "print.h"
#include "port.h"

#define PIT_FREQUENCY 1193182

static volatile uint64_t uptime_ticks = 0;

void timer_callback() {
    uptime_ticks++;
}

void timer_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;

    outb(0x43, 0x36);

    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    register_interrupt_handler(32, timer_callback);
}

uint64_t get_system_uptime() {
    return uptime_ticks / 100;  // 100 Hz
}
