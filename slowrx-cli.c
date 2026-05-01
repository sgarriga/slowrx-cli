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
#include "fsk.h"

static char bmp_name[PATH_MAX + 1] = "result.bmp";
uint8_t verbose = 0;
bool flip_x = false;
bool flip_y = false;
static double rate = 0.0; // can be used to adjust slant
static int skip = 0;	  // can be used to adjust slant

static int cmp_double(const void *a, const void *b)
{
	double da = *(const double *)a;
	double db = *(const double *)b;
	if (da < db) return -1;
	if (da > db) return 1;
	return 0;
}

static double measure_line_time(const sstv_mode_spec_t *mode_spec)
{
	int window_size = 1024;
	int step = 64; // finer time resolution for sync interval measurement
	int sync_bin = get_bin(1200.0 + shift, window_size);

	size_t available_samples = (wav_sample_count > current_sample) ? wav_sample_count - current_sample : 0;
	if (available_samples <= (size_t)window_size)
		return mode_spec->line_time;

	size_t num_windows = (available_samples - window_size) / step + 1;
	double *sync_power = calloc(num_windows, sizeof(double));
	if (!sync_power)
		return mode_spec->line_time;

	for (size_t i = 0; i < num_windows; i++) {
		size_t start_sample = current_sample + i * step;
		for (int j = 0; j < window_size; j++) {
			double window = 0.5 * (1.0 - cos(2.0 * M_PI * j / (window_size - 1)));
			fftw_in[j] = wav_samples[start_sample + j] * window;
		}
		fftw_execute(fftw_plan1024);
		sync_power[i] = power(fftw_out[sync_bin]);
	}

	// Smooth the sync power envelope to help peak detection
	for (size_t i = 1; i + 1 < num_windows; i++) {
		sync_power[i] = (sync_power[i - 1] + sync_power[i] + sync_power[i + 1]) / 3.0;
	}

	double sum = 0.0, sum_sq = 0.0, max_power = 0.0;
	for (size_t i = 0; i < num_windows; i++) {
		double value = sync_power[i];
		sum += value;
		sum_sq += value * value;
		if (value > max_power)
			max_power = value;
	}
	double mean = sum / num_windows;
	double variance = (sum_sq / num_windows) - (mean * mean);
	double std = sqrt(variance > 0.0 ? variance : 0.0);
	double threshold = fmax(mean + 1.5 * std, max_power * 0.25);

	size_t *peaks = calloc(num_windows, sizeof(size_t));
	size_t peak_count = 0;
	for (size_t i = 1; i < num_windows - 1; i++) {
		if (sync_power[i] > threshold &&
		    sync_power[i] > sync_power[i - 1] &&
		    sync_power[i] > sync_power[i + 1]) {
			peaks[peak_count++] = i;
		}
	}

	// Collect plausible line intervals from adjacent sync peaks
	double expected_line_time = mode_spec->line_time;
	double min_line_time = expected_line_time * 0.5;
	double max_line_time = expected_line_time * 1.5;
	size_t valid_count = 0;
	double *valid_intervals = calloc(peak_count, sizeof(double));
	for (size_t i = 1; i < peak_count; i++) {
		double interval = ((peaks[i] - peaks[i - 1]) * step) / (double)wav_sample_rate;
		if (interval >= min_line_time && interval <= max_line_time) {
			valid_intervals[valid_count++] = interval;
		}
	}

	double measured_line_time = 0.0;
	if (valid_count >= 4) {
		qsort(valid_intervals, valid_count, sizeof(double), cmp_double);
		measured_line_time = valid_intervals[valid_count / 2];
	}

	free(valid_intervals);
	free(peaks);

	if (measured_line_time <= 0.0) {
		// Fallback: use autocorrelation on the sync power envelope
		size_t lag_min = (size_t)fmax(1.0, (expected_line_time * wav_sample_rate / step) * 0.5);
		size_t lag_max = (size_t)fmin((double)num_windows - 1,
			(expected_line_time * wav_sample_rate / step) * 1.5);
		if (lag_max <= lag_min)
			lag_max = lag_min + 1;

		double best_corr = -INFINITY;
		size_t best_lag = lag_min;
		for (size_t lag = lag_min; lag <= lag_max; lag++) {
			double corr = 0.0;
			for (size_t i = 0; i + lag < num_windows; i++) {
				corr += (sync_power[i] - mean) * (sync_power[i + lag] - mean);
			}
			if (corr > best_corr) {
				best_corr = corr;
				best_lag = lag;
			}
		}
		if (best_corr > 0.0)
			measured_line_time = best_lag * step / (double)wav_sample_rate;
	}

	free(sync_power);

	if (measured_line_time > 0.0) {
		return measured_line_time;
	}

	return mode_spec->line_time;
}

static bool process_image()
{
	sstv_mode_t mode = 0;
	char id[20] = "";
	int cc = 0;
	const sstv_mode_spec_t *mode_spec = NULL;

	mode = get_VIS();
	if (mode == UNKNOWN)
	{
		fprintf(stderr, "Supported VIS not found\n");
		return false;
	}
	mode_spec = get_mode_spec(mode);
	if (mode_spec == NULL)
	{
		fprintf(stderr, "VIS spec not found %d\n", (uint8_t)mode);
		return false;
	}

	printf("==== %s ====\n", mode_spec->mode_name);

	// Allocate space for cached Lum
	lum_cache = calloc((int)((mode_spec->line_time * mode_spec->img_high + 1) * wav_sample_rate), sizeof(uint8_t));
	if (!lum_cache)
	{
		bmp_free(); // free bmp memory
		fprintf(stderr, "Unable to allocate memory for Lum\n");
		return false;
	}

	// Look for an FSK Id
	get_FSK(id);
	if (id[0])
		printf("FSK ID \"%s\"\n", id);

	// Detect actual width for Wraase modes by measuring line time
	uint16_t detected_width = mode_spec->img_wide;
	if (mode == Wraase_S2_30 || mode == Wraase_S2_60 || 
	    mode == Wraase_S2_120 || mode == Wraase_S2_180) {
		double measured_line_time = measure_line_time(mode_spec);
		// For Wraase: line_time = sync_time + porch_time + 2 * pixel_time * width
		// (since R/B channels have pixel_time/2 each, G has pixel_time)
		double sync_porch = mode_spec->sync_time + mode_spec->porch_time;
		double calculated_width = (measured_line_time - sync_porch) / (2.0 * mode_spec->pixel_time);
		detected_width = (uint16_t)round(calculated_width);

		if (verbose) {
			printf("Measured line time: %.6f s, calculated width: %d pixels\n", 
			       measured_line_time, detected_width);
		}
		
		// Sanity check: should be in a reasonable Wraase range.
		if (abs((int)detected_width - mode_spec->img_wide) <= 1) {
			detected_width = mode_spec->img_wide;
		} else {
			if (verbose) {
				printf("Adjusting from %d ", detected_width);
			}
			if (detected_width == 480)
			    detected_width = mode_spec->img_wide; // misdetection of 320
			else if (detected_width < mode_spec->img_wide)
				detected_width = 256;					// Wraase says 60 & 30 can be 256
			else if (detected_width > mode_spec->img_wide)
				detected_width = 512;					// Wraase says 120 & 180 can be 512

			if (verbose) {
				printf("to %d\n", detected_width);
			}
		}
		
	}

	cc = bmp_init(bmp_name, mode_spec->img_high, detected_width, mode_spec->row_count);

	// Decode image
	printf("get_image @ %.1f Hz, Skip %d, Shift %+d Hz\n", rate, skip, shift);
	get_image(mode_spec, detected_width, rate, skip);

    free(lum_cache);  // must be allocateed to get here
	lum_cache = NULL;

	cc = bmp_write(); // handles the free()
	if (cc)
	{
		fprintf(stderr, "Unable to write %s, err = %d\n", bmp_name, cc);
		return false;
	}

	return true;
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
		if (!strcmp("-g", argv[i]))
		{
			use_goertzel_sync = true;
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
			printf("  -g\t\tuse Goertzel algorithm for sync detection\n");
			printf("  -h\t\tshow this information\n");
			printf("  -modes\t\tlist supported SSTV modes\n");
			exit(0);
		}

		if (!strcmp("-modes", argv[i]))
		{
			printf("Supported SSTV modes:\n");
			for (int j = 0; j < (int) (sizeof(mode_spec_table) / sizeof(mode_spec_table[0])); j++)
			{
				printf("  %s\n", mode_spec_table[j].mode_name);
			}
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
	memset(fftw_in, 0, sizeof(double) * 2048);

	fftw_out = fftw_alloc_complex(2048);
	if (fftw_out == NULL)
	{
		fftw_free(fftw_in);
		fprintf(stderr, "Unable to allocate memory for FFT\n");
		exit(EXIT_FAILURE);
	}

	fftw_plan1024 = fftw_plan_dft_r2c_1d(1024, fftw_in, fftw_out, FFTW_ESTIMATE);
	fftw_plan2048 = fftw_plan_dft_r2c_1d(2048, fftw_in, fftw_out, FFTW_ESTIMATE);

	(void)process_image();

	free(wav_samples);
	wav_samples = NULL;

	fftw_destroy_plan(fftw_plan1024);
	fftw_destroy_plan(fftw_plan2048);

	fftw_free(fftw_in);
	fftw_free(fftw_out);

	exit(EXIT_SUCCESS);
}
