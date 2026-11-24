#include "idt.h"
#include "io.h"
#include "stdio.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

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
static irq_handler_t irq_routines[16] = { 0 };

extern void idt_load(uint32_t idt_ptr_addr);
extern uint32_t exc_stub_table[32];
extern uint32_t irq_stub_table[16];

static const char* exception_names[32] = {
	"Divide By Zero", "Debug", "Non-Maskable Interrupt", "Breakpoint",
	"Overflow", "Bound Range Exceeded", "Invalid Opcode", "Device Not Available",
	"Double Fault", "Coprocessor Segment Overrun", "Invalid TSS", "Segment Not Present",
	"Stack-Segment Fault", "General Protection Fault", "Page Fault", "Reserved",
	"x87 Floating-Point Exception", "Alignment Check", "Machine Check",
	"SIMD Floating-Point Exception", "Virtualization Exception",
	"Control Protection Exception", "Reserved", "Reserved", "Reserved",
	"Reserved", "Reserved", "Reserved", "Hypervisor Injection Exception",
	"VMM Communication Exception", "Security Exception", "Reserved"
};

static void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags) {
	idt[num].offset_low  = handler & 0xFFFF;
	idt[num].selector    = selector;
	idt[num].zero        = 0;
	idt[num].type_attr   = flags;
	idt[num].offset_high = (handler >> 16) & 0xFFFF;
}

static void pic_remap(void) {
	outb(PIC1_COMMAND, 0x11);
	io_wait();
	outb(PIC2_COMMAND, 0x11);
	io_wait();

	outb(PIC1_DATA, 0x20); /* Master PIC vector offset: 32 */
	io_wait();
	outb(PIC2_DATA, 0x28); /* Slave PIC vector offset: 40 */
	io_wait();

	outb(PIC1_DATA, 0x04); /* Tell Master PIC there is a Slave PIC at IRQ2 */
	io_wait();
	outb(PIC2_DATA, 0x02); /* Tell Slave PIC its cascade identity */
	io_wait();

	outb(PIC1_DATA, 0x01); /* 8086/88 mode */
	io_wait();
	outb(PIC2_DATA, 0x01);
	io_wait();

	/* Unmask all IRQs */
	outb(PIC1_DATA, 0x00);
	outb(PIC2_DATA, 0x00);
}

void irq_install_handler(uint8_t irq, irq_handler_t handler) {
	if (irq < 16) {
		irq_routines[irq] = handler;
	}
}

void irq_uninstall_handler(uint8_t irq) {
	if (irq < 16) {
		irq_routines[irq] = 0;
	}
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

	pic_remap();

	for (uint8_t i = 0; i < 16; i++) {
		idt_set_gate((uint8_t)(32 + i), irq_stub_table[i], 0x08, 0x8E);
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

void irq_handler(struct cpu_registers* regs) {
	uint8_t irq = (uint8_t)(regs->int_no - 32);

	if (irq < 16 && irq_routines[irq] != 0) {
		irq_routines[irq](regs);
	}

	if (irq >= 8) {
		outb(PIC2_COMMAND, PIC_EOI);
	}
	outb(PIC1_COMMAND, PIC_EOI);
}
