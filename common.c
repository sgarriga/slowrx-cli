/*
   Copyright (c) 2007-2013, Oona Räisänen (OH2EIQ [at] sral.fi)
   Hacked Around : 2024, Stephen Garriga

   Permission to use, copy, modify, and/or distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"

bool     adaptive  = true;
bool    *has_sync  = NULL;
uint8_t *lum_cache = NULL;

uint32_t current_sample = 0;
int shift = 0;

double       *fftw_in;
fftw_complex *fftw_out;
fftw_plan     fftw_plan1024;
fftw_plan     fftw_plan2048;

// Return the FFT bin index matching the given frequency
int get_bin (double freq, int fft_len) {
	return (freq / wav_sample_rate * fft_len);
}

// Sinusoid power from complex DFT coefficients
double power (fftw_complex coeff) {
	return pow(coeff[0],2) + pow(coeff[1],2);
}

// Clip to [0..255]
uint8_t clip (double a) {
	if      (a < 0)   return 0;
	else if (a > 255) return 255;
	return  (uint8_t)round(a);
}

// Convert degrees -> radians
double deg2rad (double deg) {
	return (deg / 180) * M_PI;
}

// Convert radians -> degrees
double rad2deg (double rad) {
	return (180 / M_PI) * rad;
}

