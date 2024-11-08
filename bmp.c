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
#define BMP_C
#include "bmp.h"

extern bool verbose;

#define BYTES_PER_PIXEL  3
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

static uint16_t height = 0;
static uint16_t width = 0;
static char *filename = NULL;

int bmp_init(char *fname, uint16_t height_in, uint16_t width_in)
{
	height = height_in;
	width = width_in;
	filename = fname;
	in_mem_bmp = (uint8_t *)calloc(height * width * BYTES_PER_PIXEL, 1);
	if (!in_mem_bmp) {
		printf("Could not allocate %d bytes\n", height * width * BYTES_PER_PIXEL);
		return -1;
	}
         if (verbose)
		printf("Allocated %p - %p for bitmap\n", in_mem_bmp, in_mem_bmp + (height * width * BYTES_PER_PIXEL) - 1);
	return 0;
}

void bmp_plot(uint16_t x, uint16_t y, BMP_RGB rgb, uint8_t val) { 
	uint8_t (*in_mem_bmp_arr)[height][width][BYTES_PER_PIXEL] = (uint8_t (*)[height][width][BYTES_PER_PIXEL])in_mem_bmp;
         if (verbose)
		printf("Bimap plot @ %p\n", in_mem_bmp_arr[y][x][(int)rgb]);
	(*in_mem_bmp_arr)[y][x][(int)rgb] = val; 
}

int bmp_write()
{
	FILE* in_mem_bmpFile = fopen(filename, "wb");
	if (!in_mem_bmpFile) {
		int cc = errno;
		printf("Could not open %s for writing\n", filename);
		return cc;
	}

	if (verbose)
		printf("Opened %s\n", filename);

	int widthInBytes = width * BYTES_PER_PIXEL;
	unsigned char padding[3] = {0, 0, 0};
	int paddingSize = (4 - (widthInBytes) % 4) % 4;
	int stride = (widthInBytes) + paddingSize;
	int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);

	if (verbose)
		printf("Writing bitmap file header\n");

	unsigned char fileHeader[FILE_HEADER_SIZE] = {
		0,0,     /// signature
		0,0,0,0, /// in_mem_bmp file size in bytes
		0,0,0,0, /// reserved
		0,0,0,0, /// start of pixel array
	};

	fileHeader[ 0] = (unsigned char)('B');
	fileHeader[ 1] = (unsigned char)('M');
	fileHeader[ 2] = (unsigned char)(fileSize      );
	fileHeader[ 3] = (unsigned char)(fileSize >>  8);
	fileHeader[ 4] = (unsigned char)(fileSize >> 16);
	fileHeader[ 5] = (unsigned char)(fileSize >> 24);
	fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

	fwrite(fileHeader, FILE_HEADER_SIZE, 1, in_mem_bmpFile);

	if (verbose)
		printf("Writing bitmap info header\n");

	unsigned char infoHeader[INFO_HEADER_SIZE] = {
		0,0,0,0, /// header size
		0,0,0,0, /// in_mem_bmp width
		0,0,0,0, /// in_mem_bmp height
		0,0,     /// number of color planes
		0,0,     /// bits per pixel
		0,0,0,0, /// compression
		0,0,0,0, /// in_mem_bmp size
		0,0,0,0, /// horizontal resolution
		0,0,0,0, /// vertical resolution
		0,0,0,0, /// colors in color table
		0,0,0,0, /// important color count
	};

	infoHeader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
	infoHeader[ 4] = (unsigned char)(width      );
	infoHeader[ 5] = (unsigned char)(width >>  8);
	infoHeader[ 6] = (unsigned char)(width >> 16);
	infoHeader[ 7] = (unsigned char)(width >> 24);
	infoHeader[ 8] = (unsigned char)(height      );
	infoHeader[ 9] = (unsigned char)(height >>  8);
	infoHeader[10] = (unsigned char)(height >> 16);
	infoHeader[11] = (unsigned char)(height >> 24);
	infoHeader[12] = (unsigned char)(1);
	infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

	fwrite(infoHeader, INFO_HEADER_SIZE, 1, in_mem_bmpFile);

	int i;
	for (i = 0; i < height; i++) {
		fwrite(in_mem_bmp + (i*widthInBytes), BYTES_PER_PIXEL, width, in_mem_bmpFile);
		fwrite(padding, 1, paddingSize, in_mem_bmpFile);
	}

	printf("Wrote %d x %d bitmap file : %s\n", height, width, filename);

	fclose(in_mem_bmpFile);
	return 0;
}
