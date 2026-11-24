.section .text

.macro IRQ irq_num, int_num
.global irq_stub_\irq_num
irq_stub_\irq_num:
	pushl $0
	pushl $\int_num
	jmp irq_common_stub
.endm

IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

.extern irq_handler
irq_common_stub:
	pusha
	mov %ds, %ax
	pushl %eax

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	pushl %esp
	call irq_handler
	add $4, %esp

	popl %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa
	add $8, %esp
	iret

.section .rodata
.global irq_stub_table
irq_stub_table:
	.long irq_stub_0,  irq_stub_1,  irq_stub_2,  irq_stub_3
	.long irq_stub_4,  irq_stub_5,  irq_stub_6,  irq_stub_7
	.long irq_stub_8,  irq_stub_9,  irq_stub_10, irq_stub_11
	.long irq_stub_12, irq_stub_13, irq_stub_14, irq_stub_15
