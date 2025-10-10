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

#include <stdio.h>
#include "common.h"

/*
 * Mode specifications
 *
 * mode_name      Long, human-readable name for the mode
 * sync_time      Duration of synchronization pulse in seconds
 * porch_time     Duration of sync porch pulse in seconds
 * sep_time       Duration of channel separator pulse in seconds
 * pixel_time     Duration of one pixel in seconds
 * line_time      Time in seconds from the beginning of a sync pulse to the beginning of the next one
 * img_wide       Pixels per scanline
 * img_high       Number of scanlines
 * row_count      Height of one scanline in pixels (1 or 2)
 * color_enc      Color format (GBR, RGB, YUV, BW)
 * channels	      Number of color channels per scanline (1, 2, 3, or 4)
 *
 *
 * Note that these timings do not fully describe the workings of the different modes.
 *
 * References:
 *
 *             JL Barber N7CXI (2000): "Proposal for SSTV Mode Specifications". Presented at the
 *             Dayton SSTV forum, 20 May 2000.
 *
 *             Dave Jones KB4YZ (1999): "SSTV modes - line timing".
 *             <http://www.tima.com/~djones/line.txt>
 */

const _mode_spec mode_spec[] = {

	[Martin_1] = {// N7CXI, 2000
			.mode_name = "Martin Martin_1",
			.sync_time = 4.862e-3,
			.porch_time = 0.572e-3,
			.sep_time = 0.572e-3,
			.pixel_time = 0.4576e-3,
			.line_time = 446.446e-3,
			.img_wide = 320,
			.img_high = 256,
			.row_count = 1,
			.color_enc = GBR,
			.channels = 3},

	[Martin_2] = {// N7CXI, 2000
			.mode_name = "Martin Martin_2",
			.sync_time = 4.862e-3,
			.porch_time = 0.572e-3,
			.sep_time = 0.572e-3,
			.pixel_time = 0.2288e-3,
			.line_time = 226.7986e-3,
			.img_wide = 320,
			.img_high = 256,
			.row_count = 1,
			.color_enc = GBR,
			.channels = 3},

	[Martin_3] = {// KB4YZ, 1999
			.mode_name = "Martin Martin_3",
			.sync_time = 4.862e-3,
			.porch_time = 0.572e-3,
			.sep_time = 0.572e-3,
			.pixel_time = 0.4576e-3,
			.line_time = 446.446e-3,
			.img_wide = 320,
			.img_high = 128,
			.row_count = 2,
			.color_enc = GBR,
			.channels = 3},

	[Martin_4] = {// KB4YZ, 1999
			.mode_name = "Martin Martin_4",
			.sync_time = 4.862e-3,
			.porch_time = 0.572e-3,
			.sep_time = 0.572e-3,
			.pixel_time = 0.2288e-3,
			.line_time = 226.7986e-3,
			.img_wide = 320,
			.img_high = 128,
			.row_count = 2,
			.color_enc = GBR,
			.channels = 3},

	[Scottie_1] = {// N7CXI, 2000
			.mode_name = "Scottie Scottie_1",
			.sync_time = 9e-3,
			.porch_time = 1.5e-3,
			.sep_time = 1.5e-3,
			.pixel_time = 0.4320e-3,
			.line_time = 428.38e-3,
			.img_wide = 320,
			.img_high = 256,
			.row_count = 1,
			.color_enc = GBR,
			.channels = 3},

	[Scottie2] = {// N7CXI, 2000
			.mode_name = "Scottie Scottie2",
			.sync_time = 9e-3,
			.porch_time = 1.5e-3,
			.sep_time = 1.5e-3,
			.pixel_time = 0.2752e-3,
			.line_time = 277.692e-3,
			.img_wide = 320,
			.img_high = 256,
			.row_count = 1,
			.color_enc = GBR,
			.channels = 3},

	[ScottieDX] = {// N7CXI, 2000
			 .mode_name = "Scottie DX",
			 .sync_time = 9e-3,
			 .porch_time = 1.5e-3,
			 .sep_time = 1.5e-3,
			 .pixel_time = 1.08053e-3,
			 .line_time = 1050.3e-3,
			 .img_wide = 320,
			 .img_high = 256,
			 .row_count = 1,
			 .color_enc = GBR,
			 .channels = 3},

	[Robot72] = {// N7CXI, 2000
			 .mode_name = "Robot 72",
			 .sync_time = 9e-3,
			 .porch_time = 3.0e-3,
			 .sep_time = 4.5e-3 + 1.5e-3,
			 .pixel_time = 0.215625e-3,
			 .line_time = 300e-3,
			 .img_wide = 320,
			 .img_high = 240,
			 .row_count = 1,
			 .color_enc = YUV,
			 .channels = 3},

	[Robot36] = {// N7CXI, 2000
			 .mode_name = "Robot 36",
			 .sync_time = 9e-3,
			 .porch_time = 3e-3,
			 .sep_time = 6e-3,
			 .pixel_time = 0.1375e-3,
			 .line_time = 150e-3,
			 .img_wide = 320,
			 .img_high = 240,
			 .row_count = 1,
			 .color_enc = YUV,
			 .channels = 2},

	[Robot24] = {// N7CXI, 2000
			 .mode_name = "Robot 24",
			 .sync_time = 6e-3,
			 .porch_time = 2e-3,
			 .sep_time = 4e-3,
			 .pixel_time = 0.14375e-3,
			 .line_time = 200e-3,
			 .img_wide = 320,
			 .img_high = 120,
			 .row_count = 2,
			 .color_enc = YUV,
			 .channels = 3},

	[Robot_24BW] = {// N7CXI, 2000
			   .mode_name = "Robot 24 B/W",
			   .sync_time = 7e-3,
			   .porch_time = 0e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.291e-3,
			   .line_time = 100e-3,
			   .img_wide = 320,
			   .img_high = 240,
			   .row_count = 1,
			   .color_enc = BW,
			   .channels = 1},

	[Robot_12BW] = {// N7CXI, 2000
			   .mode_name = "Robot 12 B/W",
			   .sync_time = 7e-3,
			   .porch_time = 0e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.291e-3,
			   .line_time = 100e-3,
			   .img_wide = 320,
			   .img_high = 120,
			   .row_count = 2,
			   .color_enc = BW,
			   .channels = 1},

	[Robot_8BW] = {// N7CXI, 2000
			  .mode_name = "Robot 8 B/W",
			  .sync_time = 7e-3,
			  .porch_time = 0e-3,
			  .sep_time = 0e-3,
			  .pixel_time = 0.1871875e-3,
			  .line_time = 66.9e-3,
			  .img_wide = 320,
			  .img_high = 120,
			  .row_count = 2,
			  .color_enc = BW,
			  .channels = 1},

	[Wraase_S2_60] = {// 
			  .mode_name = "Wraase SC-2 60",
			  .sync_time = 5.5225e-3,
			  .porch_time = 0.5e-3,
			  .sep_time = 0e-3,
			  .pixel_time = 0.250400641e-3, //0.244844e-3,
			  .line_time = 239.530018e-3, //240.3846154e-3,
			  .img_wide = 256,
			  .img_high = 256,
			  .row_count = 1,
			  .color_enc = RGB,
			  .channels = 3},

	[Wraase_S2_120] = {// KB4YZ, 1999
			   .mode_name = "Wraase SC-2 120",
			   .sync_time = 5.5225e-3,
			   .porch_time = 0.5e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.489039081e-3,
			   .line_time = 475.530018e-3,
			   .img_wide = 320,
			   .img_high = 256,
			   .row_count = 1,
			   .color_enc = RGB,
			   .channels = 3},

	[Wraase_S2_180] = {// N7CXI, 2000
			   .mode_name = "Wraase SC-2 180",
			   .sync_time = 5.5225e-3,
			   .porch_time = 0.5e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.734532e-3,
			   .line_time = 711.0225e-3,
			   .img_wide = 320,
			   .img_high = 256,
			   .row_count = 1,
			   .color_enc = RGB,
			   .channels = 3},

	[PD_50] = {// N7CXI, 2000
			  .mode_name = "PD-50",
			  .sync_time = 20e-3,
			  .porch_time = 2.08e-3,
			  .sep_time = 0e-3,
			  .pixel_time = 0.286e-3,
			  .line_time = 388.16e-3,
			  .img_wide = 320,
			  .img_high = 256,
			  .row_count = 1,
			  .color_enc = YUV,
			  .channels = 4},

	[PD_90] = {// N7CXI, 2000
			  .mode_name = "PD-90",
			  .sync_time = 20e-3,
			  .porch_time = 2.08e-3,
			  .sep_time = 0e-3,
			  .pixel_time = 0.532e-3,
			  .line_time = 703.04e-3,
			  .img_wide = 320,
			  .img_high = 256,
			  .row_count = 1,
			  .color_enc = YUV,
			  .channels = 4},

	[PD_120] = {// N7CXI, 2000
			   .mode_name = "PD-120",
			   .sync_time = 20e-3,
			   .porch_time = 2.08e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.19e-3,
			   .line_time = 508.48e-3,
			   .img_wide = 640,
			   .img_high = 496,
			   .row_count = 1,
			   .color_enc = YUV,
			   .channels = 4},

	[PD_160] = {// N7CXI, 2000
			   .mode_name = "PD-160",
			   .sync_time = 20e-3,
			   .porch_time = 2.08e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.382e-3,
			   .line_time = 804.416e-3,
			   .img_wide = 512,
			   .img_high = 400,
			   .row_count = 1,
			   .color_enc = YUV,
			   .channels = 4},

	[PD_180] = {// N7CXI, 2000
			   .mode_name = "PD-180",
			   .sync_time = 20e-3,
			   .porch_time = 2.08e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.286e-3,
			   .line_time = 754.24e-3,
			   .img_wide = 640,
			   .img_high = 496,
			   .row_count = 1,
			   .color_enc = YUV,
			   .channels = 4},

	[PD_240] = {// N7CXI, 2000
			   .mode_name = "PD-240",
			   .sync_time = 20e-3,
			   .porch_time = 2.08e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.382e-3,
			   .line_time = 1000e-3,
			   .img_wide = 640,
			   .img_high = 496,
			   .row_count = 1,
			   .color_enc = YUV,
			   .channels = 4},

	[PD_290] = {// N7CXI, 2000
			   .mode_name = "PD-290",
			   .sync_time = 20e-3,
			   .porch_time = 2.08e-3,
			   .sep_time = 0e-3,
			   .pixel_time = 0.286e-3,
			   .line_time = 937.28e-3,
			   .img_wide = 800,
			   .img_high = 616,
			   .row_count = 1,
			   .color_enc = YUV,
			   .channels = 4},

	[Pasokon_3] = {// N7CXI, 2000
			.mode_name = "Pasokon Pasokon_3",
			.sync_time = 5.208e-3,
			.porch_time = 1.042e-3,
			.sep_time = 1.042e-3,
			.pixel_time = 0.2083e-3,
			.line_time = 409.375e-3,
			.img_wide = 640,
			.img_high = 496,
			.row_count = 1,
			.color_enc = RGB,
			.channels = 3},

	[Pasokon_5] = {// N7CXI, 2000
			.mode_name = "Pasokon Pasokon_5",
			.sync_time = 7.813e-3,
			.porch_time = 1.563e-3,
			.sep_time = 1.563e-3,
			.pixel_time = 0.3125e-3,
			.line_time = 614.065e-3,
			.img_wide = 640,
			.img_high = 496,
			.row_count = 1,
			.color_enc = RGB,
			.channels = 3},

	[Pasokon_7] = {// N7CXI, 2000
			.mode_name = "Pasokon Pasokon_7",
			.sync_time = 10.417e-3,
			.porch_time = 2.083e-3,
			.sep_time = 2.083e-3,
			.pixel_time = 0.4167e-3,
			.line_time = 818.747e-3,
			.img_wide = 640,
			.img_high = 496,
			.row_count = 1,
			.color_enc = RGB,
			.channels = 3}

};

/*
 * Mapping of 7-bit VIS codes to modes
 *
 */

const sstv_mode_t vis_map[] = {UNKNOWN, UNKNOWN, Robot_8BW, UNKNOWN, Robot24, UNKNOWN, Robot_12BW, UNKNOWN, Robot36, UNKNOWN, Robot_24BW, UNKNOWN, Robot72, UNKNOWN, UNKNOWN, UNKNOWN,
			   UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
			   Martin_4, UNKNOWN, UNKNOWN, UNKNOWN, Martin_3, UNKNOWN, UNKNOWN, UNKNOWN, Martin_2, UNKNOWN, UNKNOWN, UNKNOWN, Martin_1, UNKNOWN, UNKNOWN, UNKNOWN,
			   UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, Wraase_S2_180, Scottie2, UNKNOWN, UNKNOWN, Wraase_S2_60, Scottie_1, UNKNOWN, UNKNOWN, Wraase_S2_120,
			   UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, ScottieDX, UNKNOWN, UNKNOWN, UNKNOWN,
			   UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, PD_50, PD_290, PD_120,
			   PD_180, PD_240, PD_160, PD_90, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,
			   UNKNOWN, Pasokon_3, Pasokon_5, Pasokon_7, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN};
