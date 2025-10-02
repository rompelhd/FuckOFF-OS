#pragma once
#include <stdint.h>

void isr_handler(int interrupt_number);
void isr_timer();
void isr_keyboard();

void register_interrupt_handler(int n, void (*handler)(void));
