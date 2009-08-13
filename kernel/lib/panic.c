// Copyright 2009 Nick Johnson

#include <lib.h>

void panic(char *message) {
	colork(0x7);
	printk("Kernel panic: %s\n", message);
	asm volatile ("cli");
	asm volatile ("hlt");
}