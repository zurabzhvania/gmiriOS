#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct cpu_registers {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags;
};

void idt_initialize(void);

#endif
