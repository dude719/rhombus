/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <ipc.h>
#include <proc.h>
#include <driver.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "keyboard.h"

void keyboard_irq (struct packet *packet, uint8_t port, uint32_t caller);
void keyboard_read(struct packet *packet, uint8_t port, uint32_t caller);

int main() {

	when(PORT_IRQ,  keyboard_irq);
	when(PORT_READ, keyboard_read);
	rirq(1);

	psend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}

void keyboard_irq(struct packet *packet, uint8_t port, uint32_t caller) {
	static bool shift = false;
	uint8_t scan;
	char c;

	if (caller == 0) {
		scan = inb(0x60);

		if (scan & 0x80) {
			if (keymap[scan & 0x7F] == '\0') {
				shift = false;
			}
		}

		else if (keymap[scan & 0x7F] == '\0') {
			shift = true;
		}

		else {
			if (shift) {
				c = keymap[scan + 58];
			}
			else {
				c = keymap[scan];
			}

			fwrite(&c, sizeof(char), 1, stdout);
			push_char(c);
		}
	}
}

void keyboard_read(struct packet *packet, uint8_t port, uint32_t caller) {
	char *data;
	size_t offset;
	
	if (!packet) {
		return;
	}

	data = pgetbuf(packet);
	offset = 0;

	for (offset = 0; offset < packet->data_length; offset++) {
		data[offset] = pop_char();
	}

	psend(PORT_REPLY, caller, packet);
}
