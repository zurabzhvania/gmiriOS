#include "stdio.h"
#include "vga.h"
#include <stdarg.h>
#include <stdint.h>

static void print_uint(uint32_t value, uint32_t base) {
	const char* digits = "0123456789abcdef";
	char buf[32];
	int i = 0;

	if (value == 0) {
		terminal_putchar('0');
		return;
	}

	while (value > 0) {
		buf[i++] = digits[value % base];
		value /= base;
	}

	while (i > 0) {
		terminal_putchar(buf[--i]);
	}
}

static void print_int(int32_t value) {
	if (value < 0) {
		terminal_putchar('-');
		uint32_t uval = (uint32_t)(-(value + 1)) + 1;
		print_uint(uval, 10);
	} else {
		print_uint((uint32_t)value, 10);
	}
}

void kprintf(const char* format, ...) {
	va_list args;
	va_start(args, format);

	for (size_t i = 0; format[i] != '\0'; i++) {
		if (format[i] != '%') {
			terminal_putchar(format[i]);
			continue;
		}

		i++;
		if (format[i] == '\0') {
			break;
		}

		switch (format[i]) {
			case 'c': {
				char c = (char)va_arg(args, int);
				terminal_putchar(c);
				break;
			}
			case 's': {
				const char* str = va_arg(args, const char*);
				terminal_writestring(str ? str : "(null)");
				break;
			}
			case 'd': {
				int val = va_arg(args, int);
				print_int((int32_t)val);
				break;
			}
			case 'u': {
				unsigned int val = va_arg(args, unsigned int);
				print_uint((uint32_t)val, 10);
				break;
			}
			case 'x': {
				unsigned int val = va_arg(args, unsigned int);
				terminal_writestring("0x");
				print_uint((uint32_t)val, 16);
				break;
			}
			case '%': {
				terminal_putchar('%');
				break;
			}
			default: {
				terminal_putchar('%');
				terminal_putchar(format[i]);
				break;
			}
		}
	}

	va_end(args);
}
