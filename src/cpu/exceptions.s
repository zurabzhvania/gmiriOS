.section .text

.global idt_load
.type idt_load, @function
idt_load:
	mov 4(%esp), %eax
	lidt (%eax)
	ret

.macro EXC_NOERR num
.global exc_stub_\num
exc_stub_\num:
	pushl $0
	pushl $\num
	jmp exception_common_stub
.endm

.macro EXC_ERR num
.global exc_stub_\num
exc_stub_\num:
	pushl $\num
	jmp exception_common_stub
.endm

EXC_NOERR 0
EXC_NOERR 1
EXC_NOERR 2
EXC_NOERR 3
EXC_NOERR 4
EXC_NOERR 5
EXC_NOERR 6
EXC_NOERR 7
EXC_ERR   8
EXC_NOERR 9
EXC_ERR   10
EXC_ERR   11
EXC_ERR   12
EXC_ERR   13
EXC_ERR   14
EXC_NOERR 15
EXC_NOERR 16
EXC_ERR   17
EXC_NOERR 18
EXC_NOERR 19
EXC_NOERR 20
EXC_ERR   21
EXC_NOERR 22
EXC_NOERR 23
EXC_NOERR 24
EXC_NOERR 25
EXC_NOERR 26
EXC_NOERR 27
EXC_NOERR 28
EXC_NOERR 29
EXC_ERR   30
EXC_NOERR 31

.extern exception_handler
exception_common_stub:
	pusha
	mov %ds, %ax
	pushl %eax

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	pushl %esp
	call exception_handler
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
.global exc_stub_table
exc_stub_table:
	.long exc_stub_0,  exc_stub_1,  exc_stub_2,  exc_stub_3
	.long exc_stub_4,  exc_stub_5,  exc_stub_6,  exc_stub_7
	.long exc_stub_8,  exc_stub_9,  exc_stub_10, exc_stub_11
	.long exc_stub_12, exc_stub_13, exc_stub_14, exc_stub_15
	.long exc_stub_16, exc_stub_17, exc_stub_18, exc_stub_19
	.long exc_stub_20, exc_stub_21, exc_stub_22, exc_stub_23
	.long exc_stub_24, exc_stub_25, exc_stub_26, exc_stub_27
	.long exc_stub_28, exc_stub_29, exc_stub_30, exc_stub_31
