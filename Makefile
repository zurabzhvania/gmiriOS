AS = i686-elf-as
CC = i686-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Isrc/include
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib

C_SRCS = src/kernel.c src/drivers/vga.c src/lib/stdio.c
OBJS = build/boot.o $(patsubst src/%.c,build/%.o,$(C_SRCS))
TARGET = build/myos.bin

all: $(TARGET)

build/boot.o: src/boot.s
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
