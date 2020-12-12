/* Copyright (c) 2020 tevador <tevador@gmail.com> */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

static inline void read_option(const char* option, int argc, const char** argv, bool* out) {
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], option) == 0) {
			*out = true;
			return;
		}
	}
	*out = false;
}

static inline void read_int_option(const char* option, int argc, const char** argv, int* out, int default_val) {
	for (int i = 0; i < argc - 1; ++i) {
		if (strcmp(argv[i], option) == 0 && (*out = atoi(argv[i + 1])) > 0) {
			return;
		}
	}
	*out = default_val;
}

static inline void read_float_option(const char* option, int argc, const char** argv, double* out, double default_val) {
	char* endptr;
	for (int i = 0; i < argc - 1; ++i) {
		if (strcmp(argv[i], option) == 0 && (*out = strtod(argv[i + 1], &endptr)) != 0.0 && *endptr == '\0') {
			return;
		}
	}
	*out = default_val;
}

static inline void read_string_option(const char* option, int argc, const char** argv, const char** out) {
	for (int i = 0; i < argc - 1; ++i) {
		if (strcmp(argv[i], option) == 0) {
			*out = argv[i + 1];
			return;
		}
	}
	*out = NULL;
}

static inline char parse_nibble(char hex) {
	hex &= ~0x20;
	return (hex & 0x40) ? hex - ('A' - 10) : hex & 0xf;
}

static inline void hex2bin(const char* in, int length, char* out) {
	for (int i = 0; i < length; i += 2) {
		char nibble1 = parse_nibble(*in++);
		char nibble2 = parse_nibble(*in++);
		*out++ = nibble1 << 4 | nibble2;
	}
}

static inline void output_hex(const char* data, int length) {
	for (unsigned i = 0; i < length; ++i)
		printf("%02x", data[i] & 0xff);
}
