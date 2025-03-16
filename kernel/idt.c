#include "idt.h"
#include "isr.h"
#include "print.h"
#include "timer.h"

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idt_descriptor;

static inline void load_idt(struct idt_ptr* idt_descriptor) {
    asm volatile ("lidt (%0)" : : "r" (idt_descriptor));
}

void set_idt_entry(int index, uint64_t handler) {
    idt[index].offset_low  = handler & 0xFFFF;
    idt[index].selector    = 0x08; // GDT
    idt[index].ist         = 0;
    idt[index].type_attr   = 0x8E; // (P=1, DPL=0, Type=E)
    idt[index].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[index].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[index].zero        = 0;
}

void idt_init() {
    int start_time = get_system_uptime(); // Start time

    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base  = (uint64_t)&idt;

    set_idt_entry(32, (uint64_t)isr_timer);
    set_idt_entry(33, (uint64_t)isr_keyboard);

    load_idt(&idt_descriptor);

    int elapsed_time = get_system_uptime() - start_time; // Finish time

    print_str("Interrupt Descriptor Table (IDT) ");

    print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
    print_str("[OK] ");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);

    print_str("[");
    print_int(elapsed_time);
    print_str(" s]\n");
}
