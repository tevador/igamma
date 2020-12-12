/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H

#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define DOUBLE_2POW64 18446744073709551616.0

/*
	Generates a pseudorandom floating point value
	uniformly distributed on [0,1).

	Requires one pseudo-random 64-bit integer.
*/
static double distr_uniform(uint64_t g) {
	return g / DOUBLE_2POW64;
}

/*
	Generates a pseudo-random floating point value with a standard normal
	distribution (mean=0 and stddev=1).

	Uses Marsaglia polar method: https://en.wikipedia.org/wiki/Marsaglia_polar_method

	Requires two pseudo-random 64-bit integers.

	Returns false in case of failure.
*/
static bool distr_normal(uint64_t g1, uint64_t g2, double* val_out) {
	double x = 2.0 * distr_uniform(g1) - 1.0;
	double y = 2.0 * distr_uniform(g2) - 1.0;
	//printf("normal: x = %g\n", x);
	//printf("normal: y = %g\n", y);
	double s = x * x + y * y;
	if (s == 0 || s >= 1.0) {
		return false;
	}
	double mag = sqrt(-2.0 * log(s) / s);
	*val_out = x * mag;
	return true;
}

typedef struct gamma_params {
	double alpha;
	double beta;
	double c;
	double d;
} gamma_params;

static bool distr_gamma_init(gamma_params* params) {
	if (params->alpha < 1.0) {
		return false;
	}
	double d = params->alpha - 1.0 / 3.0;
	double c = 1.0 / sqrt(9.0 * d);
	params->d = d;
	params->c = c;
	return true;
}

static double distr_gamma_logu_thr(double z, double d, double v) {
	return 0.5 * z * z + d - d * v + d * log(v);
}

/*
	Generates a pseudo-random floating point value with a gamma distribution.

	Uses Marsaglia and Tsang's Method:
	https://www.hongliangjie.com/2012/12/19/how-to-generate-gamma-random-variables/

	Requires three pseudo-random 64-bit integers.

	Returns false in case of failure.
*/
static bool distr_gamma(const gamma_params* params, const uint64_t g[3], double* val_out) {
	double z, u;
	double c = params->c;
	double d = params->d;
	double v;
	if (!distr_normal(g[0], g[1], &z)) {
		return false;
	}
	//printf("gamma: z = %g\n", z);
	if (z <= -1.0 / c) {
		return false;
	}
	v = 1.0 + c * z;
	v = v * v * v;
	//printf("gamma: v = %g\n", v);
	double logu_thr = distr_gamma_logu_thr(z, d, v);
	u = distr_uniform(g[2]);
	//printf("logu_thr = %g, log(u) = %g\n", logu_thr, log(u));
	if (log(u) >= logu_thr) {
		return false;
	}
	*val_out = d * v / params->beta;
	return true;
}

#endif
