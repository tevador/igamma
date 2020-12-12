/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#ifndef IGAMMA_H
#define IGAMMA_H

#include <stdint.h>

#define IGAMMA_SEED_BYTES 20
#define IGAMMA_KEYS_BYTES 32

/*
	Random number generator that outputs a random number between 0 and UINT64_MAX.
*/
typedef uint64_t igamma_rng();

typedef struct igamma_state {
	double alpha; /* shape */
	double beta;  /* scale */
	double c;     /* private */
	double d;     /* private */
	uint8_t keys[IGAMMA_KEYS_BYTES];
	uint8_t seed[IGAMMA_SEED_BYTES];
	igamma_rng* rng;
} igamma_state;

/*
	Inializes igamma_state constants c, d
*/
void igamma_init(igamma_state* state);

/*
	Inializes igamma_state constants c, d and sets the seed so that the n-th
	generated number is equal to a specific value.
*/
void igamma_init_invert(igamma_state* state, double value, unsigned n);

/*
	Generates the next gamma-distributed number.
*/
double igamma_next(igamma_state* state);

#endif
