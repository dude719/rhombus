/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <mutex.h>

/****************************************************************************
 * TT800S RNG for Flux
 *
 * This implementation based on code based on code based on the description 
 * of the TT800 Twisted GFSR RNG in "Twisted GFSR Generators II" by Makoto 
 * Matsumoto and Y. Kurita. It is a 32 bit RNG with a 2^800 period.
 */

/****************************************************************************
 * rand_state
 *
 * State of the random number generator. Note: none of these can ever be 0 
 * following correct execution of the generator, so it is assumed that the 
 * generator is not initialized iff the first element is 0.
 */

static uint32_t rand_state[25];

/****************************************************************************
 * rand_regen
 *
 * Regenerates the internal state of the random number generator, to produce
 * a new set of random numbers.
 */

static void rand_regen(void) {
	size_t i;
	register uint32_t t;

	for (i = 0; i < 18; i++) {
		rand_state[i] = 
			rand_state[i + 7] ^ 
			(rand_state[i] >> 1) ^ 
			((rand_state[i] & 1) ? 0x8EBFD028 : 0);
	}

	for (i = 18; i < 25; i++) {
		rand_state[i] =
			rand_state[i - 18] ^
			(rand_state[i] >> 1) ^
			((rand_state[i] & 1) ? 0x8EBFD028 : 0);
	}
}

/****************************************************************************
 * rand_salt
 *
 * Random sequence used to help initialize the random number generator by
 * adding entropy to the sequence generated by a LCG performed on the given
 * seed.
 */

static const uint32_t rand_salt[25] = {
	0x95F24DAB,	0x0B685125, 0xE76CCAE7, 0xAF3EC239,	0x715FAD23, 
	0x24A590AD,	0x69E4B5EF,	0xBF456141,	0x96BC1B7B,	0xA7BDF825, 
	0xC1DE75B7,	0x8858A9C9, 0x3DA87693,	0xB657F9DD,	0xFFDC8A9F,	
	0x8121DA71,	0x8B823ECB,	0x885D05F5, 0x4E20CD47,	0x5A9AD5D9,
	0x512C0C03,	0xEA857CCD,	0x4CC1D30F,	0x8891A9A1,	0xA6B7AADB
};

/****************************************************************************
 * srand
 *
 * Seeds the random number generator. The seed is run through a LCG taken
 * from the Public Domain C Library, xor'ed with the elements of rand_salt,
 * and then placed into the state of the random number generator. The
 * generator state is then regenerated, to minimize nonrandomness produced
 * by the less uniform LCG.
 *
 * If the given seed is 1, the generator is put into the same state as it was 
 * when the library was originally initialized. Or, more precisely, when the
 * random number generator is originally initialized, it is given a seed 
 * of 1.
 */

void srand(uint32_t seed) {
	size_t i;

	for (i = 0; i < 25; i++) {
		seed = seed * 0x41C64E6D + 0x3039;
		rand_state[i] = seed ^ rand_salt[i];

		/* on the off chance... */
		if (rand_state[i] == 0) {
			rand_state[i] = rand_salt[i];
		}
	}

	rand_regen();
}

/****************************************************************************
 * rand
 *
 * Returns a random number in the range 0 to RAND_MAX.
 *
 * Initializes the random number generator with a seed of 1 if it has not
 * yet been initialized. Regenerates the generator's state using rand_regen
 * if all values of the state have been used. Tempers the resulting value to
 * ensure uniform distribution across bits.
 */

uint32_t rand(void) {
	static size_t i = 0;
	uint32_t value;

	if (rand_state[0] == 0) {
		srand(1);
	}

	if (i == 25) {
		rand_regen();
		i = 0;
	}

	value = rand_state[i];
	i++;

	value ^= (value << 7)  & 0x2B5B2500;
	value ^= (value << 15) & 0xDB8B0000;
	value ^= (value >> 16);

	return value;
}
