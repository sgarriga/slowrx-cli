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

/* Demodulate the video signal & store all kinds of stuff for later stages
 *  mode:      M1, M2, S1, S2, R72, R36...
 *  rate:      exact sampling rate used
 *  skip:      number of PCM samples to skip at the beginning (for sync phase adjustment)
 *  returns:   true when finished, false when aborted
 */
bool get_image(sstv_mode_t mode, double rate, int skip)
{

	int MaxBin = 0;
	int VideoPlusNoiseBins = 0, ReceiverBins = 0, NoiseOnlyBins = 0;
	int n = 0;
	int SyncSampleNum;
	int i = 0, j = 0;
	int FFTLen = 1024, WinLength = 0;
	int SyncTargetBin;
	int SampleNum, Length;
	int x = 0, y = 0, tx = 0, k = 0;
	double Hann[7][1024] = {{0}};
	double Freq = 0;
	int NextSNRtime = 0, Nextsync_time = 0;
	double Praw, Psync;
	double Power[1024] = {0};
	double Pvideo_plus_noise = 0, Pnoise_only = 0, Pnoise = 0, Psignal = 0;
	double SNR = 0;
	double ChanStart[4] = {0}, ChanLen[4] = {0};
	uint8_t Image[800][616][3] = {{{0}}};
	uint8_t Channel = 0, WinIdx = 0;

	typedef struct
	{
		int X;
		int Y;
		int Time;
		uint8_t Channel;
		bool Last;
	} _PixelGrid;

	_PixelGrid *PixelGrid;
	PixelGrid = calloc(mode_spec[mode].img_wide * mode_spec[mode].img_high * 3, sizeof(_PixelGrid));

	// Initialize Hann windows of different lengths
	uint16_t HannLens[7] = {48, 64, 96, 128, 256, 512, 1024};
	for (j = 0; j < 7; j++)
		for (i = 0; i < HannLens[j]; i++)
			Hann[j][i] = 0.5 * (1 - cos((2 * M_PI * i) / (HannLens[j] - 1)));

	// Starting times of video channels on every line, counted from beginning of line
	switch (mode)
	{

	case R72:
	case R24:
		ChanLen[0] = mode_spec[mode].pixel_time * mode_spec[mode].img_wide * 2;
		ChanLen[1] = ChanLen[2] = mode_spec[mode].pixel_time * mode_spec[mode].img_wide;
		ChanStart[0] = mode_spec[mode].sync_time + mode_spec[mode].porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec[mode].sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec[mode].sep_time;
		break;

	case R36:
		ChanLen[0] = mode_spec[mode].pixel_time * mode_spec[mode].img_wide * 2;
		ChanLen[1] = ChanLen[2] = mode_spec[mode].pixel_time * mode_spec[mode].img_wide;
		ChanStart[0] = mode_spec[mode].sync_time + mode_spec[mode].porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec[mode].sep_time;
		ChanStart[2] = ChanStart[1];
		break;

	case S1:
	case S2:
	case SDX:
		ChanLen[0] = ChanLen[1] = ChanLen[2] = mode_spec[mode].pixel_time * mode_spec[mode].img_wide;
		ChanStart[0] = mode_spec[mode].sep_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec[mode].sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec[mode].sync_time + mode_spec[mode].porch_time;
		break;

	case PD50:
	case PD90:
	case PD120:
	case PD160:
	case PD180:
	case PD240:
	case PD290:
		ChanLen[0] = ChanLen[1] = ChanLen[2] = ChanLen[3] = mode_spec[mode].pixel_time * mode_spec[mode].img_wide;
		ChanStart[0] = mode_spec[mode].sync_time + mode_spec[mode].porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec[mode].sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec[mode].sep_time;
		ChanStart[3] = ChanStart[2] + ChanLen[2] + mode_spec[mode].sep_time;
		break;

	default:
		ChanLen[0] = ChanLen[1] = ChanLen[2] = mode_spec[mode].pixel_time * mode_spec[mode].img_wide;
		ChanStart[0] = mode_spec[mode].sync_time + mode_spec[mode].porch_time;
		ChanStart[1] = ChanStart[0] + ChanLen[0] + mode_spec[mode].sep_time;
		ChanStart[2] = ChanStart[1] + ChanLen[1] + mode_spec[mode].sep_time;
		break;
	}

	// plan ahead the time instants (in samples) at which to take pixels out
	int PixelIdx = 0;

	if (mode_spec[mode].channels == 4)
	{ // Working on PD* mode
		// Each radio frame encodes two image lines
		for (y = 0; y < mode_spec[mode].img_high; y += 2)
		{
			for (Channel = 0; Channel < mode_spec[mode].channels; Channel++)
			{
				for (x = 0; x < mode_spec[mode].img_wide; x++)
				{
					PixelGrid[PixelIdx].Time = (int)round(rate * (y / 2 * mode_spec[mode].line_time + ChanStart[Channel] +
																  mode_spec[mode].pixel_time * (x + 0.5))) +
											   skip;
					switch (Channel)
					{
					case 0:
						PixelGrid[PixelIdx].X = x;
						PixelGrid[PixelIdx].Y = y;
						PixelGrid[PixelIdx].Channel = Channel;
						PixelGrid[PixelIdx].Last = false;
						PixelIdx++;
						break;

					case 1:
					case 2:
						PixelGrid[PixelIdx].X = x;
						PixelGrid[PixelIdx].Y = y;
						PixelGrid[PixelIdx].Channel = Channel;
						PixelGrid[PixelIdx].Last = false;
						PixelIdx++;
						PixelGrid[PixelIdx].Time = PixelGrid[PixelIdx - 1].Time;
						PixelGrid[PixelIdx].X = x;
						PixelGrid[PixelIdx].Y = y + 1;
						PixelGrid[PixelIdx].Channel = Channel;
						PixelGrid[PixelIdx].Last = false;
						PixelIdx++;
						break;

					case 3:
						PixelGrid[PixelIdx].X = x;
						PixelGrid[PixelIdx].Y = y + 1;
						PixelGrid[PixelIdx].Channel = 0;
						PixelGrid[PixelIdx].Last = false;
						PixelIdx++;
						break;
					}
				}
			}
		}
		PixelGrid[PixelIdx - 1].Last = true;
	}
	else
	{
		for (y = 0; y < mode_spec[mode].img_high; y++)
		{
			for (Channel = 0; Channel < mode_spec[mode].channels; Channel++)
			{
				for (x = 0; x < mode_spec[mode].img_wide; x++)
				{

					if (mode == R36)
					{
						if (Channel == 1)
						{
							if (y % 2 == 0)
								PixelGrid[PixelIdx].Channel = 1;
							else
								PixelGrid[PixelIdx].Channel = 2;
						}
						else
							PixelGrid[PixelIdx].Channel = 0;
					}
					else
					{
						PixelGrid[PixelIdx].Channel = Channel;
					}

					PixelGrid[PixelIdx].Time = (int)round(rate * (y * mode_spec[mode].line_time + ChanStart[Channel] +
																  ((x - 0.5) / mode_spec[mode].img_wide * ChanLen[PixelGrid[PixelIdx].Channel]))) +
											   skip;
					PixelGrid[PixelIdx].X = x;
					PixelGrid[PixelIdx].Y = y;

					PixelGrid[PixelIdx].Last = false;

					PixelIdx++;
				}
			}
		}
		PixelGrid[PixelIdx - 1].Last = true;
	}

	for (k = 0; k < PixelIdx; k++)
	{
		if (PixelGrid[k].Time >= 0)
		{
			PixelIdx = k;
			break;
		}
	}

	// TODO - investigate PD modes further
	/*case PD50:
	  case PD90:
	  case PD120:
	  case PD160:
	  case PD180:
	  case PD240:
	  case PD290:
	  if (Curline_time >= ChanStart[2] + ChanLen[2]) Channel = 3; // ch 0 of even line
	  else if (Curline_time >= ChanStart[2])         Channel = 2;
	  else if (Curline_time >= ChanStart[1])         Channel = 1;
	  else                                          Channel = 0;
	  break;*/

	if (mode_spec[mode].channels == 4) // In PD* modes, each radio frame encodes two image lines
		Length = mode_spec[mode].line_time * mode_spec[mode].img_high / 2 * wav_sample_rate;
	else
		Length = mode_spec[mode].line_time * mode_spec[mode].img_high * wav_sample_rate;
	SyncTargetBin = get_bin(1200 + shift, FFTLen);
	SyncSampleNum = 0;

	// Loop through signal
	for (SampleNum = 0; SampleNum < Length; SampleNum++)
	{
		if (verbose > 1)
			printf("Line scan starting %s\n", wav_elapsed(current_sample));

		/*** Store the sync band for later adjustments ***/

		if (SampleNum == Nextsync_time)
		{

			Praw = Psync = 0;

			memset(fftw_in, 0, sizeof(double) * FFTLen);

			// Hann window
			for (i = 0; i < 64; i++)
				fftw_in[i] = wav_samples[current_sample - 32 + i] * Hann[1][i];

			fftw_execute(fftw_plan1024);

			for (i = get_bin(1500 + shift, FFTLen); i <= get_bin(2300 + shift, FFTLen); i++)
				Praw += power(fftw_out[i]);

			for (i = SyncTargetBin - 1; i <= SyncTargetBin + 1; i++)
				Psync += power(fftw_out[i]) * (1 - .5 * abs(SyncTargetBin - i));

			Praw /= (get_bin(2300 + shift, FFTLen) - get_bin(1500 + shift, FFTLen));
			Psync /= 2.0;

			// If there is more than twice the amount of power per Hz in the
			// sync band than in the video band, we have a sync signal here
			// has_sync[SyncSampleNum] = (Psync > 2*Praw);

			Nextsync_time += 13;
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
			if (mode == SDX && WinIdx < 6)
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
				// printf("Plot %d, %d, %d = %d\n", x, y, Channel, lum_cache[SampleNum]);
				Image[x][y][Channel] = lum_cache[SampleNum];

				// Some modes have R-Y & B-Y channels that are twice the height of the Y channel
				if (Channel > 0 && (mode == R36 /* || mode == R24 */))
					Image[x][y + 1][Channel] = lum_cache[SampleNum];

				if (x == mode_spec[mode].img_wide - 1 || PixelGrid[PixelIdx].Last)
				{
					if (verbose > 1)
						printf("Transfer line to BMP!\n");

					for (tx = 0; tx < mode_spec[mode].img_wide; tx++)
					{
						switch (mode_spec[mode].color_enc)
						{

						case RGB:
							bmp_plot(tx, y, RED, Image[tx][y][0]);
							bmp_plot(tx, y, GREEN, Image[tx][y][1]);
							bmp_plot(tx, y, BLUE, Image[tx][y][2]);
							break;

						case GBR:
							bmp_plot(tx, y, RED, Image[tx][y][2]);
							bmp_plot(tx, y, GREEN, Image[tx][y][0]);
							bmp_plot(tx, y, BLUE, Image[tx][y][1]);
							break;

						case YUV:
							bmp_plot(tx, y, RED, clip((100 * Image[tx][y][0] + 140 * Image[tx][y][1] - 17850) / 100.0));
							bmp_plot(tx, y, GREEN, clip((100 * Image[tx][y][0] - 71 * Image[tx][y][1] - 33 * Image[tx][y][2] + 13260) / 100.0));
							bmp_plot(tx, y, BLUE, clip((100 * Image[tx][y][0] + 178 * Image[tx][y][2] - 22695) / 100.0));
							break;

						case BW:
							bmp_plot(tx, y, RED, Image[tx][y][0]);
							bmp_plot(tx, y, GREEN, Image[tx][y][0]);
							bmp_plot(tx, y, BLUE, Image[tx][y][0]);
							break;
						}
					}
				}

				PixelIdx++;
			}
		}

		// current_sample += PixelGrid[PixelIdx].Time; // ??
		current_sample++;
	}

	free(PixelGrid);
	return true;
}
