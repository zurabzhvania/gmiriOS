.section .text
.global gdt_flush
.type gdt_flush, @function
gdt_flush:
	mov 4(%esp), %eax      /* Grab pointer to gp passed from C */
	lgdt (%eax)            /* Load the new GDT into the CPU */

	mov $0x10, %ax         /* 0x10 = byte offset of Kernel Data */
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	ljmp $0x08, $.flush    /* 0x08 = byte offset of Kernel Code */
.flush:
	ret
