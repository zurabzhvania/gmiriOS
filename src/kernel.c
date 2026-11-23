#include <stddef.h>
#include <stdint.h>

static volatile uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

/* Write a single byte to an x86 hardware I/O port */
static inline void outb(uint16_t port, uint8_t val) {
	__asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
	return fg | (bg << 4);
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t)uc | ((uint16_t)color << 8);
}

/* Move the blinking hardware VGA cursor to (row, col) */
static void terminal_update_cursor(size_t row, size_t col) {
	uint16_t pos = (uint16_t)(row * VGA_WIDTH + col);
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void terminal_clear(void) {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
		}
	}
}

/* Shift all lines up by one row and clear the bottom line */
static void terminal_scroll(void) {
	for (size_t y = 1; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
	}
	terminal_row = VGA_HEIGHT - 1;
}

static void terminal_putchar(char c) {
	if (c == '\n') {
		terminal_column = 0;
		terminal_row++;
	} else {
		VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_column] = vga_entry((unsigned char)c, terminal_color);
		terminal_column++;
		if (terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			terminal_row++;
		}
	}

	if (terminal_row == VGA_HEIGHT) {
		terminal_scroll();
	}
	terminal_update_cursor(terminal_row, terminal_column);
}

static void terminal_writestring(const char* str) {
	for (size_t i = 0; str[i] != '\0'; i++) {
		terminal_putchar(str[i]);
	}
}

static void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(0x0A, 0x00);
	terminal_clear();
	terminal_update_cursor(0, 0);
}

void kernel_main(void) {
	terminal_initialize();
	terminal_writestring("gmiriOS v0.1\n");
	terminal_writestring("VGA terminal driver initialized.\n");
	terminal_writestring("Newlines, scrolling, and hardware cursor active.\n");
}
