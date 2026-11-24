#include "vga.h"
#include "stdio.h"
#include "gdt.h"

void kernel_main(void) {
	terminal_initialize();
	gdt_initialize();

	kprintf("gmiriOS v0.3 initialized.\n");
	kprintf("GDT loaded.\n");
}
