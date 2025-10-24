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
#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdlib.h>
#include <fftw3.h>
#include <stdint.h>
#include <stdbool.h>
#include "wav.h"

extern uint8_t verbose;

extern bool adaptive;
extern uint8_t *lum_cache;
extern int shift;

extern double *fftw_in;
extern fftw_complex *fftw_out;
extern fftw_plan fftw_plan1024;
extern fftw_plan fftw_plan2048;

extern uint32_t current_sample;

double power(fftw_complex coeff);
uint8_t clip(double a);
double deg2rad(double deg);
int get_bin(double freq, int fft_len);

#endif
