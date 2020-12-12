/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "igamma.h"
#include "lambert_w.h"
#include "distributions.h"
#include "generator.h"

void igamma_init(igamma_state* state) {
	bool valid_params = distr_gamma_init((gamma_params*)state);
	assert(valid_params);
	assert(state->rng != NULL);
}

static void gamma_invert(igamma_state* state, double gamma);

void igamma_init_invert(igamma_state* state, double gamma, unsigned output_idx) {
	assert(gamma > 0);
	igamma_init(state);
	gamma_invert(state, gamma);
	uint64_t num[3];
	while (output_idx > 0) {
		gen_prev(state);
		gen_decode(state->seed, num);
		if (distr_gamma((gamma_params*)state, num, &gamma)) {
			output_idx--;
		}
	}
}

double igamma_next(igamma_state* state) {
	double x;
	uint64_t num[3];
	do {
		gen_next(state);
		gen_decode(state->seed, num);
	} while (!distr_gamma((gamma_params*)state, num, &x));
	return x;
}

static uint64_t uniform_invert(double u) {
	return (uint64_t)(u * DOUBLE_2POW64);
}

static void normal_invert(igamma_rng* rng, double normal, uint64_t out[2]) {
	double c = normal * normal / 2.0;
	double x_range = exp(-c / 2.0);
	//printf("x_range = %g\n", x_range);
	double x, x2, emw;
	uint64_t r0 = rng();
	bool y_neg = r0 >> 63;
	uint64_t gx;
	uint64_t r1 = rng();
	x = (normal > 0 ? 1.0 : -1.0) * x_range * distr_uniform(r1);
	//printf("invert: random x = %g\n", x);
	x2 = x * x;
	double z = c / x2;
	double w = lambert_w(z);
	emw = exp(-w);
	assert(x2 <= emw);
	double y = sqrt(emw - x2);
	//printf("invert: calculated y = %g\n", y);
	if (y_neg) {
		y = -y;
	}
	double s = x2 + y * y;
	assert(s >= 0x1p-104); /* anything smaller than 2^-104 will end up being 0 */
	assert(s < 1.0);
	double xi = (x + 1.0) / 2;
	double yi = (y + 1.0) / 2;
	out[0] = uniform_invert(xi);
	out[1] = uniform_invert(yi);
}

static void gamma_invert(igamma_state* state, double gamma) {
	double v = gamma * state->beta / state->d;
	//printf("gamma_invert: v = %g\n", v);
	double z = (cbrt(v) - 1.0) / state->c;
	//printf("gamma_invert: z needed: %g\n", z);
	uint64_t num[3];
	normal_invert(state->rng, z, num);
	uint64_t pad = state->rng() & 0xf00ull;
	double logu_thr = distr_gamma_logu_thr(z, state->d, v);
	//printf("gamma_invert: logu_thr = %g\n", logu_thr);
	double u_thr = exp(logu_thr);
	double u = u_thr * distr_uniform(state->rng());
	num[2] = uniform_invert(u) | pad;
	bool gamma_success = distr_gamma((gamma_params*)state, num, &gamma);
	assert(gamma_success);
	//printf("gamma_invert: %f\n", gamma);
	gen_encode(num, state->seed);
	do {
		gen_prev(state);
		gen_decode(state->seed, num);
	} while (!distr_gamma((gamma_params*)state, num, &gamma));
}

