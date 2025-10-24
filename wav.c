/*
   Copyright (c) 2024, Stephen Garriga

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
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

extern uint8_t verbose;

double *wav_samples = NULL; // will be malloc'd!
uint32_t wav_sample_rate = 0;
uint32_t wav_sample_count = 0;

char channel = 'R';

typedef struct
{
	char sub_chunk_ID[4];	 //  4  data_str[4];
	uint32_t sub_chunk_size; //  4  sound_size;
} ChunkHdr;					 // 8 bytes total

typedef struct
{
	char chunk_ID[4];	 //  4  riff_mark[4];
	uint32_t chunk_size; //  4  file_size;
	char format[4];		 //  4  wave_str[4];

	char sub_chunk1_ID[4];	  //  4  fmt_str[4];
	uint32_t sub_chunk1_size; //  4  pcm_bit_num;
	uint16_t audio_format;	  //  2  pcm_encode;
	uint16_t num_channels;	  //  2  sound_channel;
	uint32_t sample_rate;	  //  4  pcm_sample_freq;
	uint32_t byte_rate;		  //  4  byte_freq;
	uint16_t block_align;	  //  2  block_align;
	uint16_t bits_per_sample; //  2  sample_bits;

	ChunkHdr chunk;
} WavHdr; // 44 bytes total

#define le16tobe(le) (((le >> 8) & 0xff) | ((le & 0xff) << 8))

char *wav_elapsed(uint32_t current_sample)
{
	static char str[32] = "";
	double secs = (double)current_sample / (double)wav_sample_rate;

	if (secs > 60.0)
		sprintf(str, "%.4f min", secs / 60.0);
	else if (secs > 1.0)
		sprintf(str, "%.4f sec", secs);
	else
		sprintf(str, "%.4f ms", secs * 1000.0);
	return str;
}

int load_wav(char *filename)
{
	WavHdr hdr;
	FILE *f = NULL;
	size_t got = 0;

	f = fopen(filename, "rb");
	if (f == NULL)
	{
		int cc = errno;
		printf("Could not open %s - %s\n", filename, strerror(cc));
		return cc;
	}

	got = fread(&hdr, sizeof(WavHdr), 1, f);
	if (got < 1)
	{
		printf("Could not read %s - %d\n", filename, (int)got);
		return -1;
	}
	// If the file is a valid WAV file
	if (strncmp(hdr.format, "WAVE", 4) || strncmp(hdr.chunk_ID, "RIFF", 4))
	{
		fclose(f);
		printf("Not a WAV file\n");
		return -2;
	}
	if (hdr.audio_format != 0x0001)
	{
		fclose(f);
		printf("Unsupported WAV format %04x\n", hdr.audio_format);
		return -3;
	}
	if (hdr.sample_rate != 44100)
	{
		fclose(f);
		printf("%d sample rate, 44100 required\n", hdr.sample_rate);
		return -4;
	}
	wav_sample_rate = hdr.sample_rate;

	// advance over any non-"data" chunk(s)
	while (strncmp(hdr.chunk.sub_chunk_ID, "data", 4))
	{ // fact, ????
		if (verbose)
			printf("Skipping \"%4.4s\" chunk\n", hdr.chunk.sub_chunk_ID);
		fseek(f, hdr.chunk.sub_chunk_size, SEEK_CUR);
		got += fread(&hdr.chunk, sizeof(ChunkHdr), 1, f);
	}

	// Read wave data
	if (hdr.block_align == 4 || (hdr.block_align == 2 && (hdr.num_channels == 1)))
	{
		int16_t sample16[2] = {0, 0};
		// 16-bit samples
		wav_sample_count = (hdr.chunk.sub_chunk_size / hdr.num_channels) / sizeof(int16_t);
		wav_samples = calloc(wav_sample_count, sizeof(double));
		switch (hdr.block_align)
		{
		case 2:
			// 16-bit mono
			for (size_t i = 0; i < wav_sample_count && !feof(f); i++)
			{
				got += fread(sample16, sizeof(int16_t), 1, f);
				wav_samples[i] = (double)sample16[0] / (double)0x7FFF;
			}
			break;
		case 4:
			// 16-bit stereo
			for (size_t i = 0; i < wav_sample_count && !feof(f); i++)
			{
				got += fread(sample16, sizeof(int16_t), 2, f);
				switch (channel)
				{
				case 'L':
					wav_samples[i] = (double)sample16[0] / (double)0x7FFF;
					break;
				case 'R':
					wav_samples[i] = (double)sample16[1] / (double)0x7FFF;
					break;

				default:
					wav_samples[i] = (double)(sample16[0] & sample16[1]) / (double)0x7FFF;
				}
			}
			break;
		}
	}
	else
	{
		int8_t sample8[2] = {0, 0};
		// 8-bit samples
		wav_sample_count = (hdr.chunk.sub_chunk_size / hdr.num_channels) / sizeof(int8_t);
		wav_samples = calloc(wav_sample_count, sizeof(double));
		switch (hdr.block_align)
		{
		case 1:
			// 8-bit mono
			for (size_t i = 0; i < wav_sample_count && !feof(f); i++)
			{
				got += fread(sample8, sizeof(int8_t), 1, f);
				wav_samples[i] = (double)sample8[0] / (double)0x7F;
			}
			break;
		case 2:
			// 8-bit stereo
			for (size_t i = 0; i < wav_sample_count && !feof(f); i++)
			{
				got += fread(sample8, sizeof(int8_t), 2, f);
				switch (channel)
				{
				case 'L':
					wav_samples[i] = (double)sample8[0] / (double)0x7F;
					break;
				case 'R':
					wav_samples[i] = (double)sample8[1] / (double)0xFF;
					break;

				default:
					wav_samples[i] = (double)(sample8[0] & sample8[1]) / (double)0x7F;
				}
			}
			break;
		}
	}

	fclose(f);

	printf("Read %d samples, %s.\n", wav_sample_count, wav_elapsed(wav_sample_count));
	return 0;
}
