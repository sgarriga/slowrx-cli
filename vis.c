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

#include <math.h>
#include "common.h"
#include "modespec.h"
/*
 * Mapping of 7-bit VIS codes to modes
 *
 */

sstv_mode_t vis_map(uint8_t vis)
{
	switch (vis)
	{
	case Robot_8BW:
	case Robot24:
	case Robot_12BW:
	case Robot36:
	case Robot_24BW:
	case Robot72:
	case Martin_4:
	case Martin_3:
	case Martin_2:
	case Martin_1:
	case Wraase_S2_180:
	case Scottie2:
	case Wraase_S2_60:
	case Scottie_1:
	case Wraase_S2_120:
	case ScottieDX:
	case PD_50:
	case PD_290:
	case PD_120:
	case PD_180:
	case PD_240:
	case PD_160:
	case PD_90:
	case Pasokon_3:
	case Pasokon_5:
	case Pasokon_7:
		break;
	default:
		return UNKNOWN;
	}
	return (sstv_mode_t)vis;
}

/*
 *
 * Detect VIS & frequency shift
 *
 * Each bit lasts 30 ms (1323 samples)
 *
 */

sstv_mode_t get_VIS()
{

	int VIS = 0, Parity = 0, HedrPtr = 0;
	int FFTLen = 2048, i = 0, j = 0, k = 0, MaxBin = 0;
	double Power[2048] = {0}, HedrBuf[100] = {0}, tone[100] = {0}, Hann[882] = {0};
	bool got_VIS = false;
	uint8_t Bit[8] = {0}, ParityBit = 0;

	for (i = 0; i < FFTLen; i++)
		fftw_in[i] = 0;

	// Create 20ms Hann window
	for (i = 0; i < 882; i++)
		Hann[i] = 0.5 * (1 - cos((2 * M_PI * (double)i) / 881));

	printf("Searching for SSTV header\n");

	while (!got_VIS && current_sample < wav_sample_count)
	{

		if (verbose > 1)
			printf("Header/VIS scan starting %s\n", wav_elapsed(current_sample));

		// Apply Hann window
		for (i = 0; i < 882; i++)
			fftw_in[i] = wav_samples[current_sample + i] * Hann[i];

		// FFT of last 20 ms
		fftw_execute(fftw_plan2048);

		// Find the bin with most power
		MaxBin = 0;
		for (i = 0; i <= get_bin(6000, FFTLen); i++)
		{
			Power[i] = power(fftw_out[i]);
			if ((i >= get_bin(500, FFTLen) && i < get_bin(3300, FFTLen)) &&
				(MaxBin == 0 || Power[i] > Power[MaxBin]))
				MaxBin = i;
		}

		// Find the peak frequency by Gaussian interpolation
		if (MaxBin > get_bin(500, FFTLen) && MaxBin < get_bin(3300, FFTLen) &&
			Power[MaxBin] > 0 && Power[MaxBin + 1] > 0 && Power[MaxBin - 1] > 0)
			HedrBuf[HedrPtr] = MaxBin + (log(Power[MaxBin + 1] / Power[MaxBin - 1])) /
											(2 * log(pow(Power[MaxBin], 2) / (Power[MaxBin + 1] * Power[MaxBin - 1])));
		else
			HedrBuf[HedrPtr] = HedrBuf[(HedrPtr - 1) % 45];

		// In Hertz
		HedrBuf[HedrPtr] = HedrBuf[HedrPtr] / FFTLen * wav_sample_rate;

		// Header buffer holds 45 * 10 msec = 450 msec
		HedrPtr = (HedrPtr + 1) % 45;

		// Frequencies in the last 450 msec
		for (i = 0; i < 45; i++)
			tone[i] = HedrBuf[(HedrPtr + i) % 45];

		// Is there a pattern that looks like (the end of) a calibration header + VIS?
		// Tolerance ±25 Hz
		shift = 0;
		got_VIS = false;
		for (i = 0; i < 3; i++)
		{
			if (shift != 0)
				break;
			for (j = 0; j < 3; j++)
			{
				if ((tone[1 * 3 + i] > tone[0 + j] - 25 && tone[1 * 3 + i] < tone[0 + j] + 25) &&	// 1900 Hz leader
					(tone[2 * 3 + i] > tone[0 + j] - 25 && tone[2 * 3 + i] < tone[0 + j] + 25) &&	// 1900 Hz leader
					(tone[3 * 3 + i] > tone[0 + j] - 25 && tone[3 * 3 + i] < tone[0 + j] + 25) &&	// 1900 Hz leader
					(tone[4 * 3 + i] > tone[0 + j] - 25 && tone[4 * 3 + i] < tone[0 + j] + 25) &&	// 1900 Hz leader
					(tone[5 * 3 + i] > tone[0 + j] - 725 && tone[5 * 3 + i] < tone[0 + j] - 675) && // 1200 Hz start bit
																									// ...8 VIS bits...
					(tone[14 * 3 + i] > tone[0 + j] - 725 && tone[14 * 3 + i] < tone[0 + j] - 675))
				{ // 1200 Hz stop bit

					// Attempt to read VIS

					got_VIS = true;
					for (k = 0; k < 8; k++)
					{
						if (tone[6 * 3 + i + 3 * k] > tone[0 + j] - 625 && tone[6 * 3 + i + 3 * k] < tone[0 + j] - 575)
							Bit[k] = 0;
						else if (tone[6 * 3 + i + 3 * k] > tone[0 + j] - 825 && tone[6 * 3 + i + 3 * k] < tone[0 + j] - 775)
							Bit[k] = 1;
						else
						{ // erroneous bit
							got_VIS = false;
							break;
						}
					}
					if (got_VIS)
					{
						shift = tone[0 + j] - 1900;

						VIS = Bit[0] + (Bit[1] << 1) + (Bit[2] << 2) + (Bit[3] << 3) + (Bit[4] << 4) + (Bit[5] << 5) + (Bit[6] << 6);
						ParityBit = Bit[7];

						printf("VIS code = %d (0x%02X) @ %+d Hz\n", VIS, VIS, shift);

						Parity = Bit[0] ^ Bit[1] ^ Bit[2] ^ Bit[3] ^ Bit[4] ^ Bit[5] ^ Bit[6];

						if (VIS == (uint8_t)Robot_12BW)
							Parity = !Parity;

						if (Parity != ParityBit)
						{
							printf("Parity fail\n");
							got_VIS = false;
						}
						else if (vis_map(VIS) == UNKNOWN)
						{
							printf("Unknown VIS\n");
							got_VIS = false;
						}
						else
						{
							break;
						}
					}
				}
			}
		}

		current_sample += 441; // ??
	}

	// Skip the rest of the stop bit
	current_sample += 20e-3 * wav_sample_rate;

	return got_VIS ? vis_map(VIS) : UNKNOWN;
}
