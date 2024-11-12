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

#define MINSLANT 30
#define MAXSLANT 150
#define BUFLEN   4096
#define SYNCPIXLEN 1.5e-3

extern bool verbose;

extern bool      adaptive;
extern bool     *has_sync;
extern uint8_t  *lum_cache;
extern uint8_t   vis_map[];
extern int       shift;


extern double       *fftw_in;
extern fftw_complex *fftw_out;
extern fftw_plan     fftw_plan1024;
extern fftw_plan     fftw_plan2048;

extern uint32_t current_sample;

// SSTV modes
typedef enum {
	UNKNOWN=0,
	M1,    M2,   M3,    M4,
	S1,    S2,   SDX,
	R72,   R36,  R24,   R24BW, R12BW, R8BW,
	PD50,  PD90, PD120, PD160, PD180, PD240, PD290,
	P3,    P5,   P7,
	W2120, W2180
} sstv_mode_t;

// Color encodings
typedef enum {
	GBR, RGB, YUV, BW
} color_enc_t;

typedef struct mode_spec {
	char   *mode_name;
	char   *Shortmode_name;
	double  sync_time;
	double  porch_time;
	double  sep_time;
	double  pixel_time;
	double  line_time;
	uint16_t img_wide;
	uint16_t img_high;
	uint8_t  line_high;
	color_enc_t color_enc;
} _mode_spec;

extern _mode_spec mode_spec[];

double power(fftw_complex coeff);
uint8_t clip(double a);
double deg2rad(double deg);
void get_FSK(char *dest);
bool get_image(sstv_mode_t mode, double rate, int skip);
uint8_t get_VIS();
int get_bin(double freq, int fft_len);

#endif
