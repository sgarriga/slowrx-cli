/*
   Copyright (c) 2007-2013, Oona Räisänen (OH2EIQ case at:
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
#include <stdlib.h>
#include "modespec.h"

static const sstv_mode_spec_t mode_spec_table[] = {
    { Martin_1, "Martin_1", 4.862e-3, 0.572e-3, 0.572e-3, 0.4576e-3, 446.446e-3, 320, 256, 1, GBR, 3 },
    { Martin_2, "Martin_2", 4.862e-3, 0.572e-3, 0.572e-3, 0.2288e-3, 226.7986e-3, 320, 256, 1, GBR, 3 },
    { Martin_3, "Martin_3", 4.862e-3, 0.572e-3, 0.572e-3, 0.4576e-3, 446.446e-3, 320, 128, 2, GBR, 3 },
    { Martin_4, "Martin_4", 4.862e-3, 0.572e-3, 0.572e-3, 0.2288e-3, 226.7986e-3, 320, 128, 2, GBR, 3 },
    { Scottie_1, "Scottie_1", 9e-3, 1.5e-3, 1.5e-3, 0.4320e-3, 428.38e-3, 320, 256, 1, GBR, 3 },
    { Scottie2, "Scottie2", 9e-3, 1.5e-3, 1.5e-3, 0.2752e-3, 277.692e-3, 320, 256, 1, GBR, 3 },
    { ScottieDX, "Scottie DX", 9e-3, 1.5e-3, 1.5e-3, 1.08053e-3, 1050.3e-3, 320, 256, 1, GBR, 3 },
    { Robot72, "Robot 72", 9e-3, 3.0e-3, 6.0e-3, 0.215625e-3, 300e-3, 320, 240, 1, YUV, 3 },
    { Robot36, "Robot 36", 9e-3, 3e-3, 6e-3, 0.1375e-3, 150e-3, 320, 240, 1, YUV, 2 },
    { Robot24, "Robot 24", 6e-3, 2e-3, 4e-3, 0.14375e-3, 200e-3, 320, 120, 2, YUV, 3 },
    { Robot_24BW, "Robot 24 B/W", 7e-3, 0e-3, 0e-3, 0.291e-3, 100e-3, 320, 240, 1, BW, 1 },
    { Robot_12BW, "Robot 12 B/W", 7e-3, 0e-3, 0e-3, 0.291e-3, 100e-3, 320, 120, 2, BW, 1 },
    { Robot_8BW, "Robot 8 B/W", 7e-3, 0e-3, 0e-3, 0.1871875e-3, 66.9e-3, 320, 120, 2, BW, 1 },
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
    { Pasokon_3, "Pasokon_3", 5.208e-3, 1.042e-3, 1.042e-3, 0.2083e-3, 409.375e-3, 640, 496, 1, RGB, 3 },
    { Pasokon_5, "Pasokon_5", 7.813e-3, 1.563e-3, 1.563e-3, 0.3125e-3, 614.065e-3, 640, 496, 1, RGB, 3 },
    { Pasokon_7, "Pasokon_7", 10.417e-3, 2.083e-3, 2.083e-3, 0.4167e-3, 818.747e-3, 640, 496, 1, RGB, 3 },
};

const sstv_mode_spec_t *get_mode_spec(sstv_mode_t mode)
{
    for (size_t i = 0; i < sizeof(mode_spec_table) / sizeof(mode_spec_table[0]); ++i)
    {
        if (mode_spec_table[i].mode == mode)
            return &mode_spec_table[i];
    }
    return NULL;
}
