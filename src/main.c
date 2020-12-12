/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "igamma.h"
#include "format_utils.h"

static uint64_t lgc_seed;

static uint64_t lcg_random() {
	uint64_t x = 6364136223846793005ull * lgc_seed + 1442695040888963407ull;
	lgc_seed = x;
	return x;
}

void gen_output_ages(igamma_state* state, int ring_members, int real_output) {
	printf("Generated ring member ages:\n");
	for (int i = 0; i < ring_members; ++i) {
		printf("  %g %s\n", exp(igamma_next(state)),
			i == real_output ? "<------" : "");
	}
}

int main(int argc, const char** argv) {
	lgc_seed = time(NULL);

	igamma_state state = {
		.alpha = 19.28,
		.beta = 1.61,
		.keys = {
			77, 116,  10,  35,  78, 180,  10, 240,  36,  23, 113,   9,   2, 170,  46,  33,
			207, 211,  65, 224,  11,  16, 214, 253, 185, 127, 137,  38, 188,  69, 128,  56
		},
		.rng = &lcg_random
	};

	int ring_members;
	int real_output;
	double value;
	const char* seed_hex;
	bool gen, invert;

	read_option("--gen", argc, argv, &gen);
	read_option("--invert", argc, argv, &invert);

	if (gen) {
		read_int_option("--ring-members", argc, argv, &ring_members, 11);
		read_string_option("--seed", argc, argv, &seed_hex);
		if (strlen(seed_hex) != 2 * IGAMMA_SEED_BYTES) {
			printf("ERROR: seed must be %i hexadecimal characters\n", 2 * IGAMMA_SEED_BYTES);
			return 1;
		}
		hex2bin(seed_hex, 2 * IGAMMA_SEED_BYTES, state.seed);
		igamma_init(&state);
		gen_output_ages(&state, ring_members, -1);
		return 0;
	}

	if (invert) {
		read_int_option("--ring-members", argc, argv, &ring_members, 11);
		read_int_option("--real-output", argc, argv, &real_output, 0);
		read_float_option("--value", argc, argv, &value, 0.0);
		if (value == 0.0) {
			printf("ERROR: invalid or missing output age value\n");
			return 1;
		}
		if (value < 1.5 || value > 1.0e+26) {
			printf("ERROR: value is outside of the range that can be generated. Please specify a value between 1.5 and 1.0e+26.\n");
			return 1;
		}
		igamma_init_invert(&state, log(value), real_output);
		printf("Generated seed: ");
		output_hex((const char*)state.seed, IGAMMA_SEED_BYTES);
		printf("\n");
		gen_output_ages(&state, ring_members, real_output);
		return 0;
	}

	printf("Usage: %s OPTIONS\n\n", argv[0]);
	printf("--invert          Generate a seed that produces a value of V as the N-th output\n");
	printf("--real-output N   The index of the output. Default: 0\n");
	printf("--value V         The value of the output (required)\n");
	printf("--gen             Generate N gamma-distributed output ages\n");
	printf("--ring-members M  Specify the number of ring members. Default: 11\n");
	printf("--seed S          Specify a 20-byte seed value in hex format (required)\n");

	return 0;
}
