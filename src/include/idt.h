#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct cpu_registers {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags;
};

typedef void (*irq_handler_t)(struct cpu_registers* regs);

void idt_initialize(void);
void irq_install_handler(uint8_t irq, irq_handler_t handler);
void irq_uninstall_handler(uint8_t irq);

#endif
