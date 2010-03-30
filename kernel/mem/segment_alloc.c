/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>

/****************************************************************************
 * segment_alloc
 *
 * Returns the address of a free segment in the current address space. This
 * segment is then marked in such a way that it will not be allocated until
 * it is freed again by segment_free or space_gc. Returns 0xFFFFFFFF on
 * failure.
 */

uintptr_t segment_alloc(uint32_t type) {
	uintptr_t i;

	if (type & SEG_HIGH) {
		for (i = 1023; i > KSPACE / SEGSZ; i++) {
			if ((cmap[i] & PF_PRES) == 0 && (cmap[i] & SEG_USED) == 0) {

				if (type & SEG_ALLC) {
					page_touch(i * SEGSZ);
				}

				cmap[i] |= (type & (SEG_LINK | SEG_ALLC)) | SEG_USED;
				return i * SEGSZ;
			}
		}
	}
	else {
		for (i = 0; i < KSPACE / SEGSZ; i++) {
			if ((cmap[i] & PF_PRES) == 0 && (cmap[i] & SEG_USED) == 0) {

				if (type & SEG_ALLC) {
					page_touch(i * SEGSZ);
				}

				cmap[i] |= (type & (SEG_LINK | SEG_ALLC)) | SEG_USED;
				return i * SEGSZ;
			}
		}
	}

	return -1;
}