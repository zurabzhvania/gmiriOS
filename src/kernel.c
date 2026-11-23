#include "vga.h"
#include "stdio.h"

void kernel_main(void) {
	terminal_initialize();

	kprintf("gmiriOS v0.%d initialized.\n", 2);
	kprintf("VGA buffer mapped at %x\n", 0xB8000);
	kprintf("Signed test: %d | String test: %s\n", -42, "OK");
}
