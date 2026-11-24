#include "vga.h"
#include "stdio.h"
#include "gdt.h"
#include "idt.h"

static volatile uint32_t timer_ticks = 0;

static void timer_callback(struct cpu_registers* regs) {
	(void)regs;
	timer_ticks++;
	if (timer_ticks == 1) {
		kprintf("IRQ0 timer tick received from hardware!\n");
	}
}

void kernel_main(void) {
	terminal_initialize();
	gdt_initialize();
	idt_initialize();

	irq_install_handler(0, timer_callback);
	__asm__ volatile ("sti");

	kprintf("gmiriOS v0.5 initialized.\n");
	kprintf("Hardware interrupts enabled.\n");

	for (;;) {
		__asm__ volatile ("hlt");
	}
}
