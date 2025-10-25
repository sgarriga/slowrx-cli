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
#include <stdlib.h>
#include <stdbool.h>
#include "bmp.h"

extern uint8_t verbose;
extern bool flip_x;
extern bool flip_y;

#define BYTES_PER_PIXEL 3
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

static uint16_t height = 0;	  // scanned bitmap height
static uint8_t row_count = 0; // number of row repeats per scanline
static uint16_t o_height = 0; // output bitmap height
static uint16_t width = 0;
static char *filename = NULL;
static uint8_t *in_mem_bmp = NULL; // will be malloc'd!

uint8_t bmp_init(char *fname, uint16_t height_in, uint16_t width_in, uint8_t row_count_in)
{
	height = height_in;
	row_count = row_count_in;
	width = width_in;
	o_height = height * row_count;
	filename = fname;
	in_mem_bmp = (uint8_t *)calloc(height * width * BYTES_PER_PIXEL, 2); // TODO figure out why 2 is needed here
	if (!in_mem_bmp)
	{
		printf("Could not allocate %d bytes\n", height * width * BYTES_PER_PIXEL);
		return -1;
	}
	if (verbose)
		printf("Allocated %d bytes - %p - %p for bitmap\n", (height * width * BYTES_PER_PIXEL), in_mem_bmp, in_mem_bmp + (height * width * BYTES_PER_PIXEL) - 1);
	return 0;
}

void bmp_plot(uint16_t x, uint16_t y, BMP_RGB rgb, uint8_t val)
{
	uint8_t (*in_mem_bmp_arr)[height][width][BYTES_PER_PIXEL] = (uint8_t (*)[height][width][BYTES_PER_PIXEL])in_mem_bmp;

	if (x > width || y > height)
	{
		printf("Bitmap plot out of range: %d, %d\n", x, y);
		return;
	}
	
	if (flip_x)
		x = width - x;
	if (!flip_y)
		y = height - y;

	if (verbose > 1) {
		printf("Bitmap plot @ %p [%d, %d]\n", in_mem_bmp_arr[y][x][(uint8_t)rgb], x, y);
		fflush(stdout);
	}

	(*in_mem_bmp_arr)[y][x][(uint8_t)rgb] = val;
}

uint8_t bmp_write()
{
	FILE *bmp_file = fopen(filename, "wb");
	if (!bmp_file)
	{
		int cc = errno;
		printf("Could not open %s for writing\n", filename);
		return cc;
	}

	if (verbose > 1)
		printf("Opened %s\n", filename);

	int widthInBytes = width * BYTES_PER_PIXEL;
	unsigned char padding[3] = {0, 0, 0};
	int paddingSize = (4 - (widthInBytes) % 4) % 4;
	int stride = (widthInBytes) + paddingSize;
	int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

	if (verbose > 1)
		printf("Writing bitmap file header\n");

	BITMAPFILEHEADER fileHeader = {
		.bfType = 0x4d42, // "BM" = Bitmap!
		.bfSize = htole32(fileSize),
		.bfReserved1 = 0,
		.bfReserved2 = 0,
		.bfOffBits = htole32(FILE_HEADER_SIZE + INFO_HEADER_SIZE)
	};

	fwrite(&fileHeader, sizeof(fileHeader), 1, bmp_file);

	if (verbose > 1)
		printf("Writing bitmap info header\n");

	BITMAPINFOHEADER infoHeader = {
		.biSize = htole32(INFO_HEADER_SIZE),
		.biWidth = htole32(width),
		.biHeight = htole32(o_height),
		.biPlanes = htole16(1),
		.biBitCount = htole16(BYTES_PER_PIXEL * 8),
		.biCompression = 0,
		.biSizeImage = 0,
		.biXPelsPerMeter = 0,
		.biYPelsPerMeter = 0,
		.biClrUsed = 0,
		.biClrImportant = 0
	};

	fwrite(&infoHeader, sizeof(infoHeader), 1, bmp_file);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < row_count; j++)
		{
			fwrite(in_mem_bmp + (i * widthInBytes), BYTES_PER_PIXEL, width, bmp_file);
			fwrite(padding, 1, paddingSize, bmp_file);
		}
	}

	printf("Wrote %d x %d bitmap file : %s\n", width, o_height, filename);

	fclose(bmp_file);
	bmp_free();
	return 0;
}

void bmp_free() {
	if (in_mem_bmp) {
		free(in_mem_bmp);
		in_mem_bmp = NULL;
	}
}
