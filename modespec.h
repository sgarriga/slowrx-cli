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
#ifndef _MODESPEC_H_
#define _MODESPEC_H_
#include <stdint.h>

// SSTV modes/VIS codes - not all are implemented
typedef enum
{
        UNKNOWN = 0xff, // 0 is used for Robot12 - needs to be an unused uint8_t value

        // Martin modes
        Martin_1 = 44,
        Martin_2 = 40,
        Martin_3 = 36,
        Martin_4 = 32,

        // Scottie modes
        Scottie_1 = 60,
        Scottie2 = 56,
        ScottieDX = 76,

        // Robot modes
        Robot72 = 12,
        Robot36 = 8,
        Robot24 = 4,
        Robot12 = 0,
        Robot_24BW = 10, // green
        Robot_12BW = 6,  // green
        Robot_8BW = 2,   // green

        // PD modes
        PD_50 = 93,
        PD_90 = 99,
        PD_120 = 95,
        PD_160 = 98,
        PD_180 = 96,
        PD_240 = 97,
        PD_290 = 94,

        // Pasokon modes
        Pasokon_3 = 113,
        Pasokon_5 = 114,
        Pasokon_7 = 115,

        // Wraase modes
        Wraase_S2_30 = 51,
        Wraase_S2_60 = 59,
        Wraase_S2_120 = 63,
        Wraase_S2_180 = 55
} sstv_mode_t;

// Color encodings
typedef enum
{
        GBR,
        RGB,
        YUV,
        BW
} color_enc_t;

typedef struct mode_spec
{
        sstv_mode_t mode;
        char *mode_name;
        double sync_time;
        double porch_time;
        double sep_time;
        double pixel_time;
        double line_time;
        uint16_t img_wide;
        uint16_t img_high;
        uint8_t row_count;
        color_enc_t color_enc;
        uint8_t channels;
} sstv_mode_spec_t;

sstv_mode_spec_t *get_mode_spec(sstv_mode_t mode);
sstv_mode_t get_VIS();

#endif
