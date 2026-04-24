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
        Scottie_2 = 56,
        Scottie_DX = 76,

        // Robot modes
        Robot_72 = 12,
        Robot_36 = 8,
        Robot_24 = 4,
        Robot_12 = 0,
        Robot_24_BW = 10, // green
        Robot_12_BW = 6,  // green
        Robot_8_BW = 2,   // green

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
        uint8_t channels; /* count of transmitted components or image channels */
} sstv_mode_spec_t;


static const sstv_mode_spec_t mode_spec_table[] = {
    { Martin_1, "Martin 1", 4.862e-3, 0.572e-3, 0.572e-3, 0.4576e-3, 446.446e-3, 320, 256, 1, GBR, 3 },
    { Martin_2, "Martin 2", 4.862e-3, 0.572e-3, 0.572e-3, 0.2288e-3, 226.7986e-3, 320, 256, 1, GBR, 3 },
    { Martin_3, "Martin 3", 4.862e-3, 0.572e-3, 0.572e-3, 0.4576e-3, 446.446e-3, 320, 128, 2, GBR, 3 },
    { Martin_4, "Martin 4", 4.862e-3, 0.572e-3, 0.572e-3, 0.2288e-3, 226.7986e-3, 320, 128, 2, GBR, 3 },
    { Scottie_1, "Scottie 1", 9e-3, 1.5e-3, 1.5e-3, 0.4320e-3, 428.38e-3, 320, 256, 1, GBR, 3 },
    { Scottie_2, "Scottie 2", 9e-3, 1.5e-3, 1.5e-3, 0.2752e-3, 277.692e-3, 320, 256, 1, GBR, 3 },
    { Scottie_DX, "Scottie DX", 9e-3, 1.5e-3, 1.5e-3, 1.08053e-3, 1050.3e-3, 320, 256, 1, GBR, 3 },
    { Robot_72, "Robot 72", 9e-3, 3.0e-3, 6.0e-3, 0.215625e-3, 300e-3, 320, 240, 1, YUV, 3 },
    { Robot_36, "Robot 36", 9e-3, 3e-3, 6e-3, 0.1375e-3, 150e-3, 320, 240, 1, YUV, 2 },
    { Robot_24, "Robot 24", 6e-3, 2e-3, 4e-3, 0.14375e-3, 200e-3, 320, 120, 2, YUV, 3 },
    { Robot_12, "Robot 12", 9e-3, 3e-3, 6e-3, 0.125e-3, 100e-3, 320, 240, 1, YUV, 3 },
    { Robot_24_BW, "Robot 24 B/W", 7e-3, 0e-3, 0e-3, 0.291e-3, 100e-3, 320, 120, 2, BW, 1 },
    { Robot_12_BW, "Robot 12 B/W", 7e-3, 0e-3, 0e-3, 0.291e-3, 100e-3, 320, 120, 2, BW, 1 },
    { Robot_8_BW, "Robot 8 B/W", 7e-3, 0e-3, 0e-3, 0.1871875e-3, 66.9e-3, 320, 120, 2, BW, 1 },
    { Wraase_S2_30, "Wraase SC-2 30", 5.5225e-3, 0.5e-3, 0e-3, 0.183e-3, 120.192e-3, 320, 256, 1, RGB, 3 },
    { Wraase_S2_60, "Wraase SC-2 60", 5.5225e-3, 0.5e-3, 0e-3, 0.366191e-3, 240.3846154e-3, 320, 256, 1, RGB, 3 },
    { Wraase_S2_120, "Wraase SC-2 120", 5.5225e-3, 0.5e-3, 0e-3, 0.733606e-3, 475.5300178e-3, 320, 256, 1, RGB, 3 },
    { Wraase_S2_180, "Wraase SC-2 180", 5.5225e-3, 0.5e-3, 0e-3, 0.734375e-3, 711.0225e-3, 320, 256, 1, RGB, 3 },
    { PD_50, "PD-50", 20e-3, 2.08e-3, 0e-3, 0.286e-3, 388.16e-3, 320, 256, 1, YUV, 4 },
    { PD_90, "PD-90", 20e-3, 2.08e-3, 0e-3, 0.532e-3, 703.04e-3, 320, 256, 1, YUV, 4 },
    { PD_120, "PD-120", 20e-3, 2.08e-3, 0e-3, 0.19e-3, 508.48e-3, 640, 496, 1, YUV, 4 },
    { PD_160, "PD-160", 20e-3, 2.08e-3, 0e-3, 0.382e-3, 804.416e-3, 512, 400, 1, YUV, 4 },
    { PD_180, "PD-180", 20e-3, 2.08e-3, 0e-3, 0.286e-3, 754.24e-3, 640, 496, 1, YUV, 4 },
    { PD_240, "PD-240", 20e-3, 2.08e-3, 0e-3, 0.382e-3, 1000e-3, 640, 496, 1, YUV, 4 },
    { PD_290, "PD-290", 20e-3, 2.08e-3, 0e-3, 0.286e-3, 937.28e-3, 800, 616, 1, YUV, 4 },
    { Pasokon_3, "Pasokon 3", 5.208e-3, 1.042e-3, 1.042e-3, 0.2083e-3, 409.375e-3, 640, 496, 1, RGB, 3 },
    { Pasokon_5, "Pasokon 5", 7.813e-3, 1.563e-3, 1.563e-3, 0.3125e-3, 614.065e-3, 640, 496, 1, RGB, 3 },
    { Pasokon_7, "Pasokon 7", 10.417e-3, 2.083e-3, 2.083e-3, 0.4167e-3, 818.747e-3, 640, 496, 1, RGB, 3 },
};

const sstv_mode_spec_t *get_mode_spec(sstv_mode_t mode);
sstv_mode_t get_VIS();

#endif
