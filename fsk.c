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
#include <string.h>
#include "common.h"

/*
 *
 * Decode FSK ID
 *
 * * The FSK IDs are 6-bit bytes, LSB first, 45.45 baud (22 ms/bit), 1900 Hz = 1, 2100 Hz = 0
 * * Text data starts with 20 2A and ends in 01
 * * Add 0x20 and the data becomes ASCII
 *
 */

void get_FSK(char *dest)
{

	int FFTLen = 2048, i = 0, LoBin, HiBin, MidBin, TestNum = 0, TestPtr = 0;
	uint8_t Bit = 0, AsciiByte = 0, BytePtr = 0, TestBits[24] = {0}, BitPtr = 0;
	double HiPow, LoPow, Hann[970];
	bool InSync = false;
	size_t started_at = current_sample;

	// Bit-reversion lookup table
	static const uint8_t BitRev[] = {
		0x00, 0x20, 0x10, 0x30, 0x08, 0x28, 0x18, 0x38,
		0x04, 0x24, 0x14, 0x34, 0x0c, 0x2c, 0x1c, 0x3c,
		0x02, 0x22, 0x12, 0x32, 0x0a, 0x2a, 0x1a, 0x3a,
		0x06, 0x26, 0x16, 0x36, 0x0e, 0x2e, 0x1e, 0x3e,
		0x01, 0x21, 0x11, 0x31, 0x09, 0x29, 0x19, 0x39,
		0x05, 0x25, 0x15, 0x35, 0x0d, 0x2d, 0x1d, 0x3d,
		0x03, 0x23, 0x13, 0x33, 0x0b, 0x2b, 0x1b, 0x3b,
		0x07, 0x27, 0x17, 0x37, 0x0f, 0x2f, 0x1f, 0x3f};

	for (i = 0; i < FFTLen; i++)
		fftw_in[i] = 0;

	// Create 22ms Hann window
	for (i = 0; i < 970; i++)
		Hann[i] = 0.5 * (1 - cos(2 * M_PI * i / 969.0));

	while (current_sample < wav_sample_count)
	{

		if (verbose > 1)
			printf("FSK scan starting %s\n", wav_elapsed(current_sample));

		// Apply Hann window
		for (i = 0; i < 970; i++)
			fftw_in[i] = wav_samples[current_sample + i] * Hann[i];

		current_sample += (InSync ? 970 : 485);

		// FFT of last 22 ms
		fftw_execute(fftw_plan2048);

		LoBin = get_bin(1900 + shift, FFTLen) - 1;
		MidBin = get_bin(2000 + shift, FFTLen);
		HiBin = get_bin(2100 + shift, FFTLen) + 1;

		LoPow = 0;
		HiPow = 0;

		for (i = LoBin; i <= HiBin; i++)
		{
			if (i < MidBin)
				LoPow += power(fftw_out[i]);
			else
				HiPow += power(fftw_out[i]);
		}

		Bit = (LoPow > HiPow);

		if (!InSync)
		{

			// Wait for 20 2A

			TestBits[TestPtr % 24] = Bit;

			TestNum = 0;
			for (i = 0; i < 12; i++)
				TestNum |= TestBits[(TestPtr - (23 - i * 2)) % 24] << (11 - i);

			if (BitRev[(TestNum >> 6) & 0x3f] == 0x20 && BitRev[TestNum & 0x3f] == 0x2a)
			{
				InSync = true;
				AsciiByte = 0;
				BitPtr = 0;
				BytePtr = 0;
			}

			if (++TestPtr > 200)
				break;
		}
		else
		{

			AsciiByte |= Bit << BitPtr;

			if (++BitPtr == 6)
			{
				if (AsciiByte < 0x0d || BytePtr > 9)
					break;
				dest[BytePtr] = AsciiByte + 0x20;
				BitPtr = 0;
				AsciiByte = 0;
				BytePtr++;
			}
		}
	}

	dest[BytePtr] = '\0';

	// if we didn't see an FSH Id then rewind to where we started looking
	if (!*dest)
	{
		current_sample = started_at;
	}
}
