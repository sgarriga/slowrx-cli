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
#include <string.h>
#include "common.h"
#include "bmp.h"
#include "image.h"

typedef struct
{
	int X;
	int Y;
	int Time;
	uint8_t Channel;
	bool Last;
} PixelGrid;

static inline size_t image_index(int x, int y, int width)
{
	return ((size_t)y * width + x) * 3;
}

static bool allocate_image_buffer(int width, int height, uint8_t **buffer, size_t *size)
{
	*size = (size_t)width * height * 3;
	*buffer = calloc(1, *size);
	return *buffer != NULL;
}

static bool allocate_pixel_grid(size_t count, PixelGrid **grid)
{
	*grid = calloc(count, sizeof(PixelGrid));
	return *grid != NULL;
}

static void init_hann_windows(double Hann[7][1024], const uint16_t HannLens[7])
{
	int i, j;
	for (j = 0; j < 7; j++)
	{
		for (i = 0; i < HannLens[j]; i++)
		{
			Hann[j][i] = 0.5 * (1 - cos((2 * M_PI * i) / (HannLens[j] - 1)));
		}
	}
}

static void compute_channel_timing(const sstv_mode_spec_t *mode_spec, double ChanStart[4], double ChanLen[4])
{
	switch (mode_spec->mode)
	{
	case Robot_72:
	case Robot_24:
	case Robot_12:
		ChanLen[0] = mode_spec->pixel_time * mode_spec->img_wide * 2;
		ChanLen[1] = ChanLen[2] = mode_spec->pixel_time * mode_spec->img_wide;
		ChanStart[0] = mode_spec->sync_time + mode_spec->porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec->sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec->sep_time;
		break;

	case Robot_36:
		ChanLen[0] = mode_spec->pixel_time * mode_spec->img_wide * 2;
		ChanLen[1] = ChanLen[2] = mode_spec->pixel_time * mode_spec->img_wide;
		ChanStart[0] = mode_spec->sync_time + mode_spec->porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec->sep_time;
		ChanStart[2] = ChanStart[1];
		break;

	case Scottie_1:
	case Scottie_2:
	case Scottie_DX:
		ChanLen[0] = ChanLen[1] = ChanLen[2] = mode_spec->pixel_time * mode_spec->img_wide;
		ChanStart[0] = mode_spec->sep_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec->sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec->sync_time + mode_spec->porch_time;
		break;

	case PD_50:
	case PD_90:
	case PD_120:
	case PD_160:
	case PD_180:
	case PD_240:
	case PD_290:
		ChanLen[0] = ChanLen[1] = ChanLen[2] = ChanLen[3] = mode_spec->pixel_time * mode_spec->img_wide;
		ChanStart[0] = mode_spec->sync_time + mode_spec->porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec->sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec->sep_time;
		ChanStart[3] = ChanStart[2] + ChanLen[2] + mode_spec->sep_time;
		break;

	case Wraase_S2_60:
	case Wraase_S2_120:
	case Wraase_S2_30:
		ChanLen[0] = ChanLen[2] = (mode_spec->pixel_time / 2.0) * mode_spec->img_wide;
		ChanLen[1] = mode_spec->pixel_time * mode_spec->img_wide;
		ChanStart[0] = mode_spec->sync_time + mode_spec->porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec->sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec->sep_time;
		break;

	default:
		ChanLen[0] = ChanLen[1] = ChanLen[2] = mode_spec->pixel_time * mode_spec->img_wide;
		ChanStart[0] = mode_spec->sync_time + mode_spec->porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec->sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec->sep_time;
		break;
	}
}

static double goertzel_detect(double *samples, int num_samples, double target_freq, double sample_rate)
{
	// Goertzel algorithm for detecting a specific frequency
	// Returns the magnitude squared of the frequency component
	
	int k = (int)(0.5 + ((double)num_samples * target_freq) / sample_rate);
	double omega = (2.0 * M_PI * k) / num_samples;
	double sine = sin(omega);
	double cosine = cos(omega);
	double coeff = 2.0 * cosine;
	
	double q0 = 0.0, q1 = 0.0, q2 = 0.0;
	
	for (int i = 0; i < num_samples; i++)
	{
		q0 = coeff * q1 - q2 + samples[i];
		q2 = q1;
		q1 = q0;
	}
	
	// Calculate magnitude squared
	double real = q1 - q2 * cosine;
	double imag = q2 * sine;
	return real * real + imag * imag;
}

static bool build_pixel_grid(const sstv_mode_spec_t *mode_spec, double rate, int skip,
				const double ChanStart[4], const double ChanLen[4],
				PixelGrid *grid, size_t *pixel_count)
{
	size_t idx = 0;
	int x, y;
	uint8_t Channel;

	if (mode_spec->channels == 4)
	{
		for (y = 0; y < mode_spec->img_high; y += 2)
		{
			for (Channel = 0; Channel < mode_spec->channels; Channel++)
			{
				for (x = 0; x < mode_spec->img_wide; x++)
				{
					int sample_time = (int)round(rate * (y / 2 * mode_spec->line_time + ChanStart[Channel] +
						mode_spec->pixel_time * (x + 0.5))) + skip;

					grid[idx].Time = sample_time;
					grid[idx].X = x;
					grid[idx].Y = y;
					grid[idx].Last = false;

					switch (Channel)
					{
					case 0:
						grid[idx].Channel = 0;
						idx++;
						break;

					case 1:
					case 2:
						grid[idx].Channel = Channel;
						idx++;
						grid[idx].Time = sample_time;
						grid[idx].X = x;
						grid[idx].Y = y + 1;
						grid[idx].Channel = Channel;
						grid[idx].Last = false;
						idx++;
						break;

					case 3:
						grid[idx].Channel = 0;
						grid[idx].Y = y + 1;
						idx++;
						break;
					}
				}
			}
		}
	}
	else
	{
		for (y = 0; y < mode_spec->img_high; y++)
		{
			for (Channel = 0; Channel < mode_spec->channels; Channel++)
			{
				for (x = 0; x < mode_spec->img_wide; x++)
				{
					if (mode_spec->mode == Robot_36)
					{
						if (Channel == 1)
							grid[idx].Channel = (y % 2 == 0) ? 1 : 2;
						else
							grid[idx].Channel = 0;
					}
					else
					{
						grid[idx].Channel = Channel;
					}

					grid[idx].Time = (int)round(rate * (y * mode_spec->line_time + ChanStart[Channel] +
						((x - 0.5) / mode_spec->img_wide * ChanLen[grid[idx].Channel]))) + skip;
					grid[idx].X = x;
					grid[idx].Y = y;
					grid[idx].Last = false;
					idx++;
				}
			}
		}
	}

	if (idx == 0)
		return false;

	grid[idx - 1].Last = true;
	*pixel_count = idx;
	return true;
}

/* Demodulate the video signal & store all kinds of stuff for later stages
 *  mode:      Martin_1, Martin_2, Scottie_1, Scottie2, Robot72, Robot36...
 *  rate:      exact sampling rate used
 *  skip:      number of PCM samples to skip at the beginning (for sync phase adjustment)
 *  returns:   true when finished, false when aborted
 */
bool get_image(const sstv_mode_spec_t *mode_spec, double rate, int skip)
{

	int MaxBin = 0;
	int VideoPlusNoiseBins = 0, ReceiverBins = 0, NoiseOnlyBins = 0;
	int n = 0;
	int search_window = 1000; // samples to search for sync around expected
	int expected_sync = 0;
	double max_Psync_search = 0;
	int best_sample = 0;
	int searching = 0;
	int i = 0;
	int FFTLen = 1024, WinLength = 0;
	int SyncTargetBin;
	int SampleNum, Length;
	int SyncSampleNum = 0;
	int x = 0, y = 0, tx = 0, k = 0, PixelIdx = 0;
	double Hann[7][1024] = {{0}};
	double Freq = 0;
	int NextSNRtime = 0, Nextsync_time = 0;
	double Praw, Psync;
	double Power[1024] = {0};
	double Pvideo_plus_noise = 0, Pnoise_only = 0, Pnoise = 0, Psignal = 0;
	double SNR = 0;
	double ChanStart[4] = {0}, ChanLen[4] = {0};
	uint8_t *Image = NULL;
	size_t img_size = 0;
	uint8_t Channel = 0, WinIdx = 0;
	uint16_t HannLens[7] = {48, 64, 96, 128, 256, 512, 1024};
	PixelGrid *PixelGrid = NULL;
	size_t pixel_count = 0;

	if (!allocate_image_buffer(mode_spec->img_wide, mode_spec->img_high, &Image, &img_size))
	{
		fprintf(stderr, "Failed to allocate memory for image\n");
		return false;
	}

	if (!allocate_pixel_grid((size_t)mode_spec->img_wide * mode_spec->img_high * 3, &PixelGrid))
	{
		free(Image);
		fprintf(stderr, "Unable to allocate memory for PixelGrid\n");
		return false;
	}

	init_hann_windows(Hann, HannLens);
	compute_channel_timing(mode_spec, ChanStart, ChanLen);

	if (!build_pixel_grid(mode_spec, rate, skip, ChanStart, ChanLen, PixelGrid, &pixel_count))
	{
		free(Image);
		free(PixelGrid);
		return false;
	}

	for (k = 0; k < (int)pixel_count; k++)
	{
		if (PixelGrid[k].Time >= 0)
		{
			PixelIdx = k;
			break;
		}
	}


	if (mode_spec->channels == 4) // In PD* modes, each radio frame encodes two image lines
		Length = mode_spec->line_time * mode_spec->img_high / 2 * wav_sample_rate;
	else
		Length = mode_spec->line_time * mode_spec->img_high * wav_sample_rate;
	SyncTargetBin = get_bin(1200 + shift, FFTLen);
	SyncSampleNum = 0;

	// Initialize sync search
	expected_sync = 0;
	searching = 1;
	Nextsync_time = expected_sync - search_window;
	if (Nextsync_time < 0) Nextsync_time = 0;
	max_Psync_search = 0;
	best_sample = expected_sync;

	// Loop through signal
	for (SampleNum = 0; SampleNum < Length; SampleNum++)
	{
		if (verbose > 1)
			printf("Line scan starting %s\n", wav_elapsed(current_sample));

		/*** Store the sync band for later adjustments ***/

		// Start sync search if close to expected
		if (!searching && SampleNum >= expected_sync - 50 && SampleNum <= expected_sync + 50)
		{
			searching = 1;
			Nextsync_time = expected_sync - search_window;
			if (Nextsync_time < 0) Nextsync_time = 0;
			max_Psync_search = 0;
			best_sample = expected_sync;
		}

		if (SampleNum == Nextsync_time)
		{
			if (use_goertzel_sync)
			{
				// Use Goertzel algorithm for sync detection
				int sync_win_len = 256;
				int start_idx = current_sample - sync_win_len/2;
				if (start_idx < 0) start_idx = 0;
				
				// Calculate sync power using Goertzel
				Psync = goertzel_detect(&wav_samples[start_idx], sync_win_len, 1200.0 + shift, wav_sample_rate);
				
				// Calculate video band power using Goertzel for comparison
				Praw = 0;
				for (int freq = 1500; freq <= 2300; freq += 100) // Sample a few frequencies in video band
				{
					Praw += goertzel_detect(&wav_samples[start_idx], sync_win_len, freq + shift, wav_sample_rate);
				}
				Praw /= 9.0; // Average over 9 frequency points
			}
			else
			{
				// Original FFT-based sync detection
				Praw = Psync = 0;

				memset(fftw_in, 0, sizeof(double) * FFTLen);

				// Hann window - use longer window for better sync detection
				int sync_win_len = 256; // increased from 64
				for (i = 0; i < sync_win_len; i++)
					fftw_in[i] = wav_samples[current_sample - sync_win_len/2 + i] * Hann[4][i]; // use Hann[4] for 256

				fftw_execute(fftw_plan1024);

				for (i = get_bin(1500 + shift, FFTLen); i <= get_bin(2300 + shift, FFTLen); i++)
					Praw += power(fftw_out[i]);

				for (i = SyncTargetBin - 2; i <= SyncTargetBin + 2; i++) // wider bin range
					Psync += power(fftw_out[i]) * (1 - 0.5 * abs(SyncTargetBin - i));

				Praw /= (get_bin(2300 + shift, FFTLen) - get_bin(1500 + shift, FFTLen));
				Psync /= 3.0; // adjusted for 5 bins
			}

			// Sync search
			if (searching)
			{
				if (Psync > max_Psync_search)
				{
					max_Psync_search = Psync;
					best_sample = SampleNum;
				}
				Nextsync_time += 13;
				if (Nextsync_time > expected_sync + search_window)
				{
					searching = 0;
					Nextsync_time = best_sample + (int)(mode_spec->line_time * wav_sample_rate);
					expected_sync = Nextsync_time;
				}
			}
			else
			{
				// If there is more than twice the amount of power per Hz in the
				// sync band than in the video band, we have a sync signal here
				// has_sync[SyncSampleNum] = (Psync > 2*Praw);

				Nextsync_time += 13;
			}
			SyncSampleNum++;
		}

		/*** Estimate SNR ***/

		if (SampleNum == NextSNRtime)
		{

			memset(fftw_in, 0, sizeof(double) * FFTLen);

			// Apply Hann window
			for (i = 0; i < FFTLen; i++)
				fftw_in[i] = wav_samples[current_sample + i - FFTLen / 2] * Hann[6][i]; // ??

			fftw_execute(fftw_plan1024);

			// Calculate video-plus-noise power (1500-2300 Hz)

			Pvideo_plus_noise = 0;
			for (n = get_bin(1500 + shift, FFTLen); n <= get_bin(2300 + shift, FFTLen); n++)
				Pvideo_plus_noise += power(fftw_out[n]);

			// Calculate noise-only power (400-800 Hz + 2700-3400 Hz)

			Pnoise_only = 0;
			for (n = get_bin(400 + shift, FFTLen); n <= get_bin(800 + shift, FFTLen); n++)
				Pnoise_only += power(fftw_out[n]);

			for (n = get_bin(2700 + shift, FFTLen); n <= get_bin(3400 + shift, FFTLen); n++)
				Pnoise_only += power(fftw_out[n]);

			// Bandwidths
			VideoPlusNoiseBins = get_bin(2300, FFTLen) - get_bin(1500, FFTLen) + 1;

			NoiseOnlyBins = get_bin(800, FFTLen) - get_bin(400, FFTLen) + 1 +
							get_bin(3400, FFTLen) - get_bin(2700, FFTLen) + 1;

			ReceiverBins = get_bin(3400, FFTLen) - get_bin(400, FFTLen);

			// Eq 15
			Pnoise = Pnoise_only * (1.0 * ReceiverBins / NoiseOnlyBins);
			Psignal = Pvideo_plus_noise - Pnoise_only * (1.0 * VideoPlusNoiseBins / NoiseOnlyBins);

			// Lower bound to -20 dB
			SNR = ((Psignal / Pnoise < .01) ? -20 : 10 * log10(Psignal / Pnoise));

			NextSNRtime += 256;
		}

		/*** FM demodulation ***/

		if (SampleNum % 6 == 0)
		{ // Take FFT every 6 samples

			// Adapt window size to SNR

			if (!adaptive)
				WinIdx = 0;

			else if (SNR >= 20)
				WinIdx = 0;
			else if (SNR >= 10)
				WinIdx = 1;
			else if (SNR >= 9)
				WinIdx = 2;
			else if (SNR >= 3)
				WinIdx = 3;
			else if (SNR >= -5)
				WinIdx = 4;
			else if (SNR >= -10)
				WinIdx = 5;
			else
				WinIdx = 6;

			// Minimum winlength can be doubled for Scottie DX
			if (mode_spec->mode == Scottie_DX && WinIdx < 6)
				WinIdx++;

			memset(fftw_in, 0, sizeof(double) * FFTLen);
			memset(Power, 0, sizeof(double) * 1024);

			// Apply window function

			WinLength = HannLens[WinIdx];
			if ((current_sample + i - WinLength / 2) > wav_sample_count)
				break;

			for (i = 0; i < WinLength; i++)
				fftw_in[i] = wav_samples[current_sample + i - WinLength / 2] * Hann[WinIdx][i]; // ??

			fftw_execute(fftw_plan1024);

			MaxBin = 0;

			// Find the bin with most power
			for (n = get_bin(1500 + shift, FFTLen) - 1; n <= get_bin(2300 + shift, FFTLen) + 1; n++)
			{

				Power[n] = power(fftw_out[n]);
				if (MaxBin == 0 || Power[n] > Power[MaxBin])
					MaxBin = n;
			}

			// Find the peak frequency by Gaussian interpolation
			if (MaxBin > get_bin(1500 + shift, FFTLen) - 1 && MaxBin < get_bin(2300 + shift, FFTLen) + 1)
			{
				Freq = MaxBin + (log(Power[MaxBin + 1] / Power[MaxBin - 1])) /
									(2 * log(pow(Power[MaxBin], 2) / (Power[MaxBin + 1] * Power[MaxBin - 1])));
				// In Hertz
				Freq = Freq / FFTLen * wav_sample_rate;
			}
			else
			{
				// Clip if out of bounds
				Freq = ((MaxBin > get_bin(1900 + shift, FFTLen)) ? 2300 : 1500) + shift;
			}

		} /* endif (SampleNum == PixelGrid[PixelIdx].Time) */

		// Calculate luminency & store for later use
		lum_cache[SampleNum] = clip((Freq - (1500 + shift)) / 3.1372549);

		if (SampleNum == PixelGrid[PixelIdx].Time)
		{

			// In PD* modes, two pixels need data from the same sample
			// Can't move on from SampleNum, until all are processed
			while (SampleNum == PixelGrid[PixelIdx].Time)
			{
				x = PixelGrid[PixelIdx].X;
				y = PixelGrid[PixelIdx].Y;
				Channel = PixelGrid[PixelIdx].Channel;

				// Store pixel
				Image[(y * mode_spec->img_wide + x) * 3 + Channel] = lum_cache[SampleNum];

				// Some modes have R-Y & B-Y channels that are twice the height of the Y channel
				if (Channel > 0 && (mode_spec->mode == Robot_36))
					Image[((y + 1) * mode_spec->img_wide + x) * 3 + Channel] = lum_cache[SampleNum];

				if (x == mode_spec->img_wide - 1 || PixelGrid[PixelIdx].Last)
				{
					if (verbose > 1)
						printf("Transfer line to BMP!\n");

					for (tx = 0; tx < mode_spec->img_wide; tx++)
					{
						switch (mode_spec->color_enc)
						{

						case RGB:
							bmp_plot(tx, y, RED, Image[(y * mode_spec->img_wide + tx) * 3 + 0]);
							bmp_plot(tx, y, GREEN, Image[(y * mode_spec->img_wide + tx) * 3 + 1]);
							bmp_plot(tx, y, BLUE, Image[(y * mode_spec->img_wide + tx) * 3 + 2]);
							break;

						case GBR:
							bmp_plot(tx, y, RED, Image[(y * mode_spec->img_wide + tx) * 3 + 2]);
							bmp_plot(tx, y, GREEN, Image[(y * mode_spec->img_wide + tx) * 3 + 0]);
							bmp_plot(tx, y, BLUE, Image[(y * mode_spec->img_wide + tx) * 3 + 1]);
							break;

						case YUV:
							bmp_plot(tx, y, RED, clip((100 * Image[(y * mode_spec->img_wide + tx) * 3 + 0] + 140 * Image[(y * mode_spec->img_wide + tx) * 3 + 1] - 17850) / 100.0));
							bmp_plot(tx, y, GREEN, clip((100 * Image[(y * mode_spec->img_wide + tx) * 3 + 0] - 71 * Image[(y * mode_spec->img_wide + tx) * 3 + 1] - 33 * Image[(y * mode_spec->img_wide + tx) * 3 + 2] + 13260) / 100.0));
							bmp_plot(tx, y, BLUE, clip((100 * Image[(y * mode_spec->img_wide + tx) * 3 + 0] + 178 * Image[(y * mode_spec->img_wide + tx) * 3 + 2] - 22695) / 100.0));
							break;

						case BW:
							bmp_plot(tx, y, RED, Image[(y * mode_spec->img_wide + tx) * 3 + 0]);
							bmp_plot(tx, y, GREEN, Image[(y * mode_spec->img_wide + tx) * 3 + 0]);
							bmp_plot(tx, y, BLUE, Image[(y * mode_spec->img_wide + tx) * 3 + 0]);
							break;
						}
					}
				}

				PixelIdx++;
			}
		}

		current_sample++;
	}

	free(PixelGrid);
	PixelGrid = NULL;
	free(Image);
	Image = NULL;
	
	return true;
}
