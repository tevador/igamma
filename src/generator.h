/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdint.h>
#include <string.h>

//TODO: portable version
#ifdef __GNUC__
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

#include "igamma.h"

static uint64_t load64(const void* ptr) {
	uint64_t x;
	memcpy(&x, ptr, sizeof(x));
	return x;
}

static void store64(void* dst, uint64_t x) {
	memcpy(dst, &x, sizeof(x));
}

#define MASK_12 ((1ull << 12)-1)
#define UMASK_12 ~MASK_12

/*
	Decodes three 52-bit integers from a 160-bit array
*/
static void gen_decode(uint8_t seed[IGAMMA_SEED_BYTES], uint64_t out[3]) {
	uint64_t a = load64(&seed[0]);
	a <<= 12;
	uint64_t b = load64(&seed[6]);
	b &= ~0xfull;
	b <<= 8;
	uint64_t c = load64(&seed[12]);
	c &= ~0xffull;
	out[0] = a;
	out[1] = b;
	out[2] = c;
}

/*
	Encodes three 52-bit integers into a 160-bit array
*/
static void gen_encode(uint64_t num[3], uint8_t seed[IGAMMA_SEED_BYTES]) {
	uint64_t a = num[0] >> 12;
	store64(&seed[0], a);
	a >>= 48;
	uint64_t b = (num[1] & UMASK_12) >> 8;
	b |= a;
	store64(&seed[6], b);
	b >>= 48;
	uint64_t c = num[2] & ~0xffull;
	c |= b;
	store64(&seed[12], c);
}

static void gen_part_next(__m128i key1, __m128i key2, uint8_t seed_bytes[16]) {
	__m128i seed = _mm_loadu_si128((const __m128i*)seed_bytes);
	seed = _mm_aesenc_si128(seed, key1);
	seed = _mm_aesenc_si128(seed, key1);
	seed = _mm_aesenclast_si128(seed, key2);
	_mm_storeu_si128((__m128i*)seed_bytes, seed);
}

static void gen_part_prev(__m128i key1i, __m128i key2, uint8_t seed_bytes[16]) {
	__m128i seed = _mm_loadu_si128((const __m128i*)seed_bytes);
	seed = _mm_xor_si128(seed, key2);
	seed = _mm_aesdec_si128(seed, key1i);
	seed = _mm_aesdec_si128(seed, key1i);
	seed = _mm_aesdeclast_si128(seed, _mm_setzero_si128());
	_mm_storeu_si128((__m128i*)seed_bytes, seed);
}

static void gen_next(igamma_state* state) {
	__m128i key1, key2;

	key1 = _mm_loadu_si128((const __m128i*)&state->keys[0]);
	key2 = _mm_loadu_si128((const __m128i*)&state->keys[16]);

	gen_part_next(key1, key2, &state->seed[0]);
	gen_part_next(key1, key2, &state->seed[4]);
}

static void gen_prev(igamma_state* state) {
	__m128i key1, key2;

	key1 = _mm_loadu_si128((const __m128i*)&state->keys[0]);
	key1 = _mm_aesimc_si128(key1);
	key2 = _mm_loadu_si128((const __m128i*)&state->keys[16]);

	gen_part_prev(key1, key2, &state->seed[4]);
	gen_part_prev(key1, key2, &state->seed[0]);
}

#endif
