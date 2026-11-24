AS = i686-elf-as
CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib

ASM_SRCS = src/boot.s src/cpu/gdt_flush.s src/cpu/exceptions.s src/cpu/irq.s
C_SRCS = src/kernel.c src/drivers/vga.c src/lib/stdio.c src/cpu/gdt.c src/cpu/idt.c
OBJS = $(patsubst src/%.s,build/%.o,$(ASM_SRCS)) $(patsubst src/%.c,build/%.o,$(C_SRCS))
TARGET = build/myos.bin

all: $(TARGET)

build/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET): $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lgcc

run: $(TARGET)
	qemu-system-i386 -kernel $(TARGET)

clean:
	rm -rf build

.PHONY: all run clean
