.section .text

.global idt_flush
.type idt_flush, @function
idt_flush:
	mov 4(%esp), %eax
	lidt (%eax)
	ret

/* Macro for CPU exceptions that do NOT push an error code automatically */
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
	cli
	pushl $0
	pushl $\num
	jmp isr_common_stub
.endm

/* Macro for CPU exceptions that DO push an error code automatically */
.macro ISR_ERRCODE num
.global isr\num
isr\num:
	cli
	pushl $\num
	jmp isr_common_stub
.endm

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_ERRCODE   21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

.extern isr_handler
isr_common_stub:
	pusha                  /* Save edi, esi, ebp, esp, ebx, edx, ecx, eax */

	mov %ds, %ax
	pushl %eax             /* Save current data segment */

	mov $0x10, %ax         /* Switch to Kernel Data segment (0x10 from GDT) */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	pushl %esp             /* Pass pointer to struct registers into C */
	call isr_handler
	add $4, %esp           /* Clean up pushed pointer */

	popl %eax              /* Restore original data segment */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	popa                   /* Restore general-purpose registers */
	add $8, %esp           /* Clean up pushed error code and interrupt number */
	iret                   /* Return from interrupt */
