/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#include "csprng.h"
#include <string.h>

#define CHACHA20_STATE_SIZE 64
#define CHACHA20_WORDCOUNT (CHACHA20_STATE_SIZE / sizeof(uint32_t))

typedef struct chacha20_state {
    uint32_t w[CHACHA20_WORDCOUNT];
} chacha20_state;

static void chacha20_init(chacha20_state* state, uint8_t seed[CSPRNG_SEED_SIZE]) {
	/* RFC 7539, section 2.3 */
	state->w[0] = 0x61707865;
	state->w[1] = 0x3320646e;
	state->w[2] = 0x79622d32;
	state->w[3] = 0x6b206574;
	memcpy(&state->w[4], seed, CSPRNG_SEED_SIZE);
	state->w[12] = 1;
	state->w[13] = 0x09000000;
	state->w[14] = 0x4a000000;
	state->w[15] = 0x00000000;
}

static inline uint32_t rotl(uint32_t x, unsigned int y) {
	return (x << y) | (x >> (-y & 31));
}

#define QUARTER_ROUND(a, b, c, d) (a) += (b); (d) = rotl((d) ^ (a), 16); \
								  (c) += (d); (b) = rotl((b) ^ (c), 12); \
								  (a) += (b); (d) = rotl((d) ^ (a), 8);  \
								  (c) += (d); (b) = rotl((b) ^ (c), 7)

static void chacha20_block(chacha20_state* ws, const chacha20_state* state) {
	memcpy(ws->w, state->w, CHACHA20_STATE_SIZE);
	for (int i = 0; i < 10; ++i) {
		QUARTER_ROUND(ws->w[0], ws->w[4], ws->w[8], ws->w[12]);
		QUARTER_ROUND(ws->w[1], ws->w[5], ws->w[9], ws->w[13]);
		QUARTER_ROUND(ws->w[2], ws->w[6], ws->w[10], ws->w[14]);
		QUARTER_ROUND(ws->w[3], ws->w[7], ws->w[11], ws->w[15]);
		QUARTER_ROUND(ws->w[0], ws->w[5], ws->w[10], ws->w[15]);
		QUARTER_ROUND(ws->w[1], ws->w[6], ws->w[11], ws->w[12]);
		QUARTER_ROUND(ws->w[2], ws->w[7], ws->w[8], ws->w[13]);
		QUARTER_ROUND(ws->w[3], ws->w[4], ws->w[9], ws->w[14]);
	}
	for (int i = 0; i < CHACHA20_WORDCOUNT; ++i) {
		ws->w[i] += state->w[i];
	}
}

static void chacha20_gen(chacha20_state* state, uint8_t out[]) {
	chacha20_state ws;
	chacha20_block(&ws, state);
	state->w[12]++;
	memcpy(out, ws.w, CHACHA20_STATE_SIZE);
}

static chacha20_state csprng_state;
static uint64_t csprng_numbers[CHACHA20_WORDCOUNT / 2];
static size_t csprng_available = 0;

void csprng_init_seed(uint8_t seed[CSPRNG_SEED_SIZE]) {
	chacha20_init(&csprng_state, seed);
}

uint64_t csprng_gen() {
	if (csprng_available == 0) {
		chacha20_gen(&csprng_state, (uint8_t*)&csprng_numbers);
		csprng_available = CHACHA20_WORDCOUNT / 2;
	}
	uint64_t next = csprng_numbers[CHACHA20_WORDCOUNT / 2 - csprng_available];
	csprng_available--;
	return next;
}
