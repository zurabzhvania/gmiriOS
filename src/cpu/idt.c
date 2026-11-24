#include "idt.h"
#include "stdio.h"

struct idt_gate {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t  zero;
	uint8_t  type_attr;
	uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

static struct idt_gate idt[256];
static struct idt_ptr idtp;

extern void idt_load(uint32_t idt_ptr_addr);
extern uint32_t exc_stub_table[32];

static const char* exception_names[32] = {
	"Divide By Zero",
	"Debug",
	"Non-Maskable Interrupt",
	"Breakpoint",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"Reserved",
	"x87 Floating-Point Exception",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Control Protection Exception",
	"Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
	"Hypervisor Injection Exception",
	"VMM Communication Exception",
	"Security Exception",
	"Reserved"
};

static void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
	idt[num].offset_low  = handler & 0xFFFF;
	idt[num].selector    = selector;
	idt[num].zero        = 0;
	idt[num].type_attr   = flags;
	idt[num].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_initialize(void) {
	idtp.limit = (sizeof(struct idt_gate) * 256) - 1;
	idtp.base  = (uint32_t)&idt;

	for (int i = 0; i < 256; i++) {
		idt_set_gate((uint8_t)i, 0, 0, 0);
	}

	for (uint8_t i = 0; i < 32; i++) {
		idt_set_gate(i, exc_stub_table[i], 0x08, 0x8E);
	}

	idt_load((uint32_t)&idtp);
}

void exception_handler(struct cpu_registers* regs) {
	kprintf("\n[CPU EXCEPTION %u] %s\n", regs->int_no, exception_names[regs->int_no]);
	kprintf("Error Code: %x | EIP: %x\n", regs->err_code, regs->eip);
	kprintf("System halted.\n");

	for (;;) {
		__asm__ volatile ("cli; hlt");
	}
}
