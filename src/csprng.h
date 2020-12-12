/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#ifndef CSPRNG_H
#define CSPRNG_H

#include <stdint.h>

#define CSPRNG_SEED_SIZE 32

void csprng_init_seed(uint8_t seed[CSPRNG_SEED_SIZE]);
uint64_t csprng_gen();

#endif
