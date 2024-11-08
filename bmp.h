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

#ifndef BMP_H
#define BMP_H
#include <stdint.h>

#ifdef BMP_C
uint8_t *in_mem_bmp = NULL; // will be malloc'd!
#else
extern uint8_t *in_mem_bmp; // will be malloc'd!
#endif

typedef enum { RED, GREEN, BLUE } BMP_RGB;

// create the in-memory bitmap 
int bmp_init(char *fname, uint16_t height_in, uint16_t width_in);

// write out the in-memory bitbap
int bmp_write();

// plot pixels in the in-memory bitmap
void bmp_plot(uint16_t x, uint16_t y, BMP_RGB rgb, uint8_t val);
#endif
