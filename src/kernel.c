#include "vga.h"
#include "stdio.h"
#include "gdt.h"
#include "idt.h"

void kernel_main(void) {
	terminal_initialize();
	gdt_initialize();
	idt_initialize();

	kprintf("gmiriOS v0.4 initialized.\n");
	kprintf("GDT and IDT loaded.\n");

	/* Trigger software breakpoint (Vector 3) to verify exception handling */
	__asm__ volatile ("int $0x03");
}
