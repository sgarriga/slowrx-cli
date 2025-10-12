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

sstv_mode_spec_t *get_mode_spec(sstv_mode_t mode)
{
	static sstv_mode_spec_t mode_spec;
	mode_spec.mode = mode;

	switch (mode)
	{
	default:
		return NULL;

	case Martin_1:
		mode_spec.mode_name = "Martin Martin_1";
		mode_spec.sync_time = 4.862e-3;
		mode_spec.porch_time = 0.572e-3;
		mode_spec.sep_time = 0.572e-3;
		mode_spec.pixel_time = 0.4576e-3;
		mode_spec.line_time = 446.446e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = GBR;
		mode_spec.channels = 3;
		break;

	case Martin_2:
		mode_spec.mode_name = "Martin Martin_2";
		mode_spec.sync_time = 4.862e-3;
		mode_spec.porch_time = 0.572e-3;
		mode_spec.sep_time = 0.572e-3;
		mode_spec.pixel_time = 0.2288e-3;
		mode_spec.line_time = 226.7986e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = GBR;
		mode_spec.channels = 3;
		break;

	case Martin_3:
		mode_spec.mode_name = "Martin Martin_3";
		mode_spec.sync_time = 4.862e-3;
		mode_spec.porch_time = 0.572e-3;
		mode_spec.sep_time = 0.572e-3;
		mode_spec.pixel_time = 0.4576e-3;
		mode_spec.line_time = 446.446e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 128;
		mode_spec.row_count = 2;
		mode_spec.color_enc = GBR;
		mode_spec.channels = 3;
		break;

	case Martin_4:
		mode_spec.mode_name = "Martin Martin_4";
		mode_spec.sync_time = 4.862e-3;
		mode_spec.porch_time = 0.572e-3;
		mode_spec.sep_time = 0.572e-3;
		mode_spec.pixel_time = 0.2288e-3;
		mode_spec.line_time = 226.7986e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 128;
		mode_spec.row_count = 2;
		mode_spec.color_enc = GBR;
		mode_spec.channels = 3;
		break;

	case Scottie_1:
		mode_spec.mode_name = "Scottie Scottie_1";
		mode_spec.sync_time = 9e-3;
		mode_spec.porch_time = 1.5e-3;
		mode_spec.sep_time = 1.5e-3;
		mode_spec.pixel_time = 0.4320e-3;
		mode_spec.line_time = 428.38e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = GBR;
		mode_spec.channels = 3;
		break;

	case Scottie2:
		mode_spec.mode_name = "Scottie Scottie2";
		mode_spec.sync_time = 9e-3;
		mode_spec.porch_time = 1.5e-3;
		mode_spec.sep_time = 1.5e-3;
		mode_spec.pixel_time = 0.2752e-3;
		mode_spec.line_time = 277.692e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = GBR;
		mode_spec.channels = 3;
		break;

	case ScottieDX:
		mode_spec.mode_name = "Scottie DX";
		mode_spec.sync_time = 9e-3;
		mode_spec.porch_time = 1.5e-3;
		mode_spec.sep_time = 1.5e-3;
		mode_spec.pixel_time = 1.08053e-3;
		mode_spec.line_time = 1050.3e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = GBR;
		mode_spec.channels = 3;
		break;

	case Robot72:
		mode_spec.mode_name = "Robot 72";
		mode_spec.sync_time = 9e-3;
		mode_spec.porch_time = 3.0e-3;
		mode_spec.sep_time = 4.5e-3 + 1.5e-3;
		mode_spec.pixel_time = 0.215625e-3;
		mode_spec.line_time = 300e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 240;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 3;
		break;

	case Robot36:
		mode_spec.mode_name = "Robot 36";
		mode_spec.sync_time = 9e-3;
		mode_spec.porch_time = 3e-3;
		mode_spec.sep_time = 6e-3;
		mode_spec.pixel_time = 0.1375e-3;
		mode_spec.line_time = 150e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 240;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 2;
		break;

	case Robot24:
		mode_spec.mode_name = "Robot 24";
		mode_spec.sync_time = 6e-3;
		mode_spec.porch_time = 2e-3;
		mode_spec.sep_time = 4e-3;
		mode_spec.pixel_time = 0.14375e-3;
		mode_spec.line_time = 200e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 120;
		mode_spec.row_count = 2;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 3;
		break;

	case Robot_24BW:
		mode_spec.mode_name = "Robot 24 B/W";
		mode_spec.sync_time = 7e-3;
		mode_spec.porch_time = 0e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.291e-3;
		mode_spec.line_time = 100e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 240;
		mode_spec.row_count = 1;
		mode_spec.color_enc = BW;
		mode_spec.channels = 1;
		break;

	case Robot_12BW:
		mode_spec.mode_name = "Robot 12 B/W";
		mode_spec.sync_time = 7e-3;
		mode_spec.porch_time = 0e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.291e-3;
		mode_spec.line_time = 100e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 120;
		mode_spec.row_count = 2;
		mode_spec.color_enc = BW;
		mode_spec.channels = 1;
		break;

	case Robot_8BW:
		mode_spec.mode_name = "Robot 8 B/W";
		mode_spec.sync_time = 7e-3;
		mode_spec.porch_time = 0e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.1871875e-3;
		mode_spec.line_time = 66.9e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 120;
		mode_spec.row_count = 2;
		mode_spec.color_enc = BW;
		mode_spec.channels = 1;
		break;

	case Wraase_S2_60:
		mode_spec.mode_name = "Wraase SC-2 60";
		mode_spec.sync_time = 5.5225e-3;
		mode_spec.porch_time = 0.5e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.45703125e-3; /* (Green) */
		mode_spec.line_time = 240.3846154e-3;
		mode_spec.img_wide = 256; 
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = RGB;
		mode_spec.channels = 3;
		break;

	case Wraase_S2_120:
		mode_spec.mode_name = "Wraase SC-2 120";
		mode_spec.sync_time = 5.5225e-3;
		mode_spec.porch_time = 0.5e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.734532e-3; /* (Green) */
		mode_spec.line_time = 475.5300178e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = RGB;
		mode_spec.channels = 3;
		break;

	case Wraase_S2_180:
		mode_spec.mode_name = "Wraase SC-2 180";
		mode_spec.sync_time = 5.5225e-3;
		mode_spec.porch_time = 0.5e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.734532e-3;
		mode_spec.line_time = 711.0225e-3;
		mode_spec.img_wide = 320; // some sources claim 512
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = RGB;
		mode_spec.channels = 3;
		break;

	case PD_50:
		mode_spec.mode_name = "PD-50";
		mode_spec.sync_time = 20e-3;
		mode_spec.porch_time = 2.08e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.286e-3;
		mode_spec.line_time = 388.16e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 4;
		break;

	case PD_90:
		mode_spec.mode_name = "PD-90";
		mode_spec.sync_time = 20e-3;
		mode_spec.porch_time = 2.08e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.532e-3;
		mode_spec.line_time = 703.04e-3;
		mode_spec.img_wide = 320;
		mode_spec.img_high = 256;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 4;
		break;

	case PD_120:
		mode_spec.mode_name = "PD-120";
		mode_spec.sync_time = 20e-3;
		mode_spec.porch_time = 2.08e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.19e-3;
		mode_spec.line_time = 508.48e-3;
		mode_spec.img_wide = 640;
		mode_spec.img_high = 496;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 4;
		break;

	case PD_160:
		mode_spec.mode_name = "PD-160";
		mode_spec.sync_time = 20e-3;
		mode_spec.porch_time = 2.08e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.382e-3;
		mode_spec.line_time = 804.416e-3;
		mode_spec.img_wide = 512;
		mode_spec.img_high = 400;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 4;
		break;

	case PD_180:
		mode_spec.mode_name = "PD-180";
		mode_spec.sync_time = 20e-3;
		mode_spec.porch_time = 2.08e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.286e-3;
		mode_spec.line_time = 754.24e-3;
		mode_spec.img_wide = 640;
		mode_spec.img_high = 496;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 4;
		break;

	case PD_240:
		mode_spec.mode_name = "PD-240";
		mode_spec.sync_time = 20e-3;
		mode_spec.porch_time = 2.08e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.382e-3;
		mode_spec.line_time = 1000e-3;
		mode_spec.img_wide = 640;
		mode_spec.img_high = 496;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 4;
		break;

	case PD_290:
		mode_spec.mode_name = "PD-290";
		mode_spec.sync_time = 20e-3;
		mode_spec.porch_time = 2.08e-3;
		mode_spec.sep_time = 0e-3;
		mode_spec.pixel_time = 0.286e-3;
		mode_spec.line_time = 937.28e-3;
		mode_spec.img_wide = 800;
		mode_spec.img_high = 616;
		mode_spec.row_count = 1;
		mode_spec.color_enc = YUV;
		mode_spec.channels = 4;
		break;

	case Pasokon_3:
		mode_spec.mode_name = "Pasokon Pasokon_3";
		mode_spec.sync_time = 5.208e-3;
		mode_spec.porch_time = 1.042e-3;
		mode_spec.sep_time = 1.042e-3;
		mode_spec.pixel_time = 0.2083e-3;
		mode_spec.line_time = 409.375e-3;
		mode_spec.img_wide = 640;
		mode_spec.img_high = 496;
		mode_spec.row_count = 1;
		mode_spec.color_enc = RGB;
		mode_spec.channels = 3;
		break;

	case Pasokon_5:
		mode_spec.mode_name = "Pasokon Pasokon_5";
		mode_spec.sync_time = 7.813e-3;
		mode_spec.porch_time = 1.563e-3;
		mode_spec.sep_time = 1.563e-3;
		mode_spec.pixel_time = 0.3125e-3;
		mode_spec.line_time = 614.065e-3;
		mode_spec.img_wide = 640;
		mode_spec.img_high = 496;
		mode_spec.row_count = 1;
		mode_spec.color_enc = RGB;
		mode_spec.channels = 3;
		break;

	case Pasokon_7:
		mode_spec.mode_name = "Pasokon Pasokon_7";
		mode_spec.sync_time = 10.417e-3;
		mode_spec.porch_time = 2.083e-3;
		mode_spec.sep_time = 2.083e-3;
		mode_spec.pixel_time = 0.4167e-3;
		mode_spec.line_time = 818.747e-3;
		mode_spec.img_wide = 640;
		mode_spec.img_high = 496;
		mode_spec.row_count = 1;
		mode_spec.color_enc = RGB;
		mode_spec.channels = 3;
		break;
	}
	return &mode_spec;
}
