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


 *
 *
 * slowrx-cli - an SSTV decoder
 * ----------------------------
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include "common.h"
#include "modespec.h"
#include "image.h"
#include "bmp.h"

static char bmp_name[PATH_MAX + 1] = "result.bmp";
uint8_t verbose = 0;
bool flip_x = false;
bool flip_y = false;
static double rate = 0.0; // can be used to adjust slant
static int skip = 0;	  // can be used to adjust slant

void process_image()
{
	sstv_mode_t mode = 0;
	char id[20] = "";
	int cc = 0;
	sstv_mode_spec_t *mode_spec = NULL;

	mode = get_VIS();
	if (mode == UNKNOWN)
	{
		fprintf(stderr, "Supported  VIS not found\n");
		exit(EXIT_FAILURE);
	}
	mode_spec = get_mode_spec(mode);
	if (mode_spec == NULL)
	{
		fprintf(stderr, "VIS spec not found %d\n", (uint8_t)mode);
		exit(EXIT_FAILURE);
	}

	printf("==== %s ====\n", mode_spec->mode_name);

	cc = bmp_init(bmp_name, mode_spec->img_high, mode_spec->img_wide, mode_spec->row_count);
	if (cc != 0)
	{
		fprintf(stderr, "Cannot allocate memory for BMP\n");
		exit(EXIT_FAILURE);
	}

	// Allocate space for cached Lum
	// free(lum_cache);
	lum_cache = calloc((int)((mode_spec->line_time * mode_spec->img_high + 1) * wav_sample_rate), sizeof(uint8_t));
	if (lum_cache == NULL)
	{
		fprintf(stderr, "Unable to allocate memory for Lum\n");
		exit(EXIT_FAILURE);
	}

	// Look for an FSK Id
	get_FSK(id);
	if (id[0])
		printf("FSK ID \"%s\"\n", id);

	// Decode image
	printf("get_image @ %.1f Hz, Skip %d, Shift %+d Hz\n", rate, skip, shift);
	get_image(mode_spec, rate, skip);

	if (lum_cache)
	{
		free(lum_cache);
	}

	cc = bmp_write();
	if (cc)
	{
		fprintf(stderr, "Unable to write %s, err = %d\n", bmp_name, cc);
		exit(EXIT_FAILURE);
	}
}

/*
 * main
 */

int main(int argc, char *argv[])
{

	char *wav = NULL;
	int cc = 0;

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp("-a", argv[i]))
		{
			adaptive = true; // default
			continue;
		}
		if (!strcmp("-na", argv[i]))
		{
			adaptive = false;
			continue;
		}
		if (!strcmp("-mx", argv[i]))
		{
			flip_x = true;
			continue;
		}
		if (!strcmp("-my", argv[i]))
		{
			flip_y = true;
			continue;
		}
		if (!strcmp("-o", argv[i]))
		{
			strncpy(bmp_name, argv[++i], PATH_MAX);
			continue;
		}
		if (!strcmp("-v", argv[i]))
		{
			verbose++;
			continue;
		}
		if (!strcmp("-r", argv[i]))
		{
			rate = atof(argv[++i]);
			continue;
		}
		if (!strcmp("-s", argv[i]))
		{
			skip = atoi(argv[++i]);
			continue;
		}
		if (!strcmp("-h", argv[i]))
		{
			printf("Usage:\n%s {opts} filename\n", argv[0]);
			printf("  -a\t\tadaptive (default)\n");
			printf("  -mx\t\tmirror X axis\n");
			printf("  -my\t\tmirror Y axis\n");
			printf("  -na\t\tnot adaptive\n");
			printf("  -o filename\toutput bitmap filename (default result.bmp)\n");
			printf("  -v\t\tverbose output\n");
			printf("  -r float\toverride WAV sample rate, Hz (default 44100)\n");
			printf("  -s int\tsync. adjustment (default 0.0)\n");
			printf("  -h\t\tshow this information\n");
			exit(0);
		}
		wav = argv[i];
	};

	if (cc)
	{
		fprintf(stderr, "Must specify input\n");
		exit(EXIT_FAILURE);
	}
	cc = load_wav(wav);
	if (cc)
	{
		fprintf(stderr, "Unable to open %s\n", wav);
		exit(EXIT_FAILURE);
	}
	current_sample = 0;
	if (rate == 0.0)
		rate = wav_sample_rate;

	// Prepare FFT
	fftw_in = fftw_alloc_real(2048);
	if (fftw_in == NULL)
	{
		fprintf(stderr, "Unable to allocate memory for FFT\n");
		exit(EXIT_FAILURE);
	}
	fftw_out = fftw_alloc_complex(2048);
	if (fftw_out == NULL)
	{
		fprintf(stderr, "Unable to allocate memory for FFT\n");
		exit(EXIT_FAILURE);
	}
	memset(fftw_in, 0, sizeof(double) * 2048);

	fftw_plan1024 = fftw_plan_dft_r2c_1d(1024, fftw_in, fftw_out, FFTW_ESTIMATE);
	fftw_plan2048 = fftw_plan_dft_r2c_1d(2048, fftw_in, fftw_out, FFTW_ESTIMATE);

	process_image();

	free(wav_samples);

	fftw_destroy_plan(fftw_plan1024);
	fftw_destroy_plan(fftw_plan2048);

	if (fftw_in)
	{
		fftw_free(fftw_in);
	}
	if (fftw_out)
	{
		fftw_free(fftw_out);
	}

	return (EXIT_SUCCESS);
}
