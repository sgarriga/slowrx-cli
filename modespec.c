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
 * mode_name          Long, human-readable name for the mode
 * Shortmode_name     Abbreviation for the mode, used in filenames
 * sync_time      Duration of synchronization pulse in seconds
 * porch_time     Duration of sync porch pulse in seconds
 * sep_time     Duration of channel separator pulse in seconds
 * pixel_time     Duration of one pixel in seconds
 * line_time      Time in seconds from the beginning of a sync pulse to the beginning of the next one
 * img_wide      Pixels per scanline
 * img_high      Number of scanlines
 * line_high    Height of one scanline in pixels (1 or 2)
 * color_enc      Color format (GBR, RGB, YUV, BW)
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

_mode_spec mode_spec[] = {

	[M1] = {  // N7CXI, 2000
		.mode_name         = "Martin M1",
		.Shortmode_name    = "M1",
		.sync_time     = 4.862e-3,
		.porch_time    = 0.572e-3,
		.sep_time    = 0.572e-3,
		.pixel_time    = 0.4576e-3,
		.line_time     = 446.446e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = GBR },

	[M2] = {  // N7CXI, 2000
		.mode_name         = "Martin M2",
		.Shortmode_name    = "M2",
		.sync_time     = 4.862e-3,
		.porch_time    = 0.572e-3,
		.sep_time    = 0.572e-3,
		.pixel_time    = 0.2288e-3,
		.line_time     = 226.7986e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = GBR },

	[M3] = {   // KB4YZ, 1999
		.mode_name         = "Martin M3",
		.Shortmode_name    = "M3",
		.sync_time     = 4.862e-3,
		.porch_time    = 0.572e-3,
		.sep_time    = 0.572e-3,
		.pixel_time    = 0.2288e-3,
		.line_time     = 446.446e-3,
		.img_wide     = 320,
		.img_high     = 128,
		.line_high   = 2,
		.color_enc     = GBR },

	[M4] = {   // KB4YZ, 1999
		.mode_name         = "Martin M4",
		.Shortmode_name    = "M4",
		.sync_time     = 4.862e-3,
		.porch_time    = 0.572e-3,
		.sep_time    = 0.572e-3,
		.pixel_time    = 0.2288e-3,
		.line_time     = 226.7986e-3,
		.img_wide     = 320,
		.img_high     = 128,
		.line_high   = 2,
		.color_enc     = GBR },

	[S1] = {  // N7CXI, 2000
		.mode_name         = "Scottie S1",
		.Shortmode_name    = "S1",
		.sync_time     = 9e-3,
		.porch_time    = 1.5e-3,
		.sep_time    = 1.5e-3,
		.pixel_time    = 0.4320e-3,
		.line_time     = 428.38e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = GBR },

	[S2] = {  // N7CXI, 2000
		.mode_name         = "Scottie S2",
		.Shortmode_name    = "S2",
		.sync_time     = 9e-3,
		.porch_time    = 1.5e-3,
		.sep_time    = 1.5e-3,
		.pixel_time    = 0.2752e-3,
		.line_time     = 277.692e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = GBR },

	[SDX] = {  // N7CXI, 2000
		.mode_name         = "Scottie DX",
		.Shortmode_name    = "SDX",
		.sync_time     = 9e-3,
		.porch_time    = 1.5e-3,
		.sep_time    = 1.5e-3,
		.pixel_time    = 1.08053e-3,
		.line_time     = 1050.3e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = GBR },

	[R72] = {  // N7CXI, 2000
		.mode_name         = "Robot 72",
		.Shortmode_name    = "R72",
		.sync_time     = 9e-3,
		.porch_time    = 3e-3,
		.sep_time    = 4.7e-3,
		.pixel_time    = 0.2875e-3,
		.line_time     = 300e-3,
		.img_wide     = 320,
		.img_high     = 240,
		.line_high   = 1,
		.color_enc     = YUV },

	[R36] = {  // N7CXI, 2000
		.mode_name         = "Robot 36",
		.Shortmode_name    = "R36",
		.sync_time     = 9e-3,
		.porch_time    = 3e-3,
		.sep_time    = 6e-3,
		.pixel_time    = 0.1375e-3,
		.line_time     = 150e-3,
		.img_wide     = 320,
		.img_high     = 240,
		.line_high   = 1,
		.color_enc     = YUV },

	[R24] = {  // N7CXI, 2000
		.mode_name         = "Robot 24",
		.Shortmode_name    = "R24",
		.sync_time     = 9e-3,
		.porch_time    = 3e-3,
		.sep_time    = 6e-3,
		.pixel_time    = 0.1375e-3,
		.line_time     = 150e-3,
		.img_wide     = 320,
		.img_high     = 240,
		.line_high   = 1,
		.color_enc     = YUV },

	[R24BW] = {  // N7CXI, 2000
		.mode_name         = "Robot 24 B/W",
		.Shortmode_name    = "R24BW",
		.sync_time     = 7e-3,
		.porch_time    = 0e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.291e-3,
		.line_time     = 100e-3,
		.img_wide     = 320,
		.img_high     = 240,
		.line_high   = 1,
		.color_enc     = BW },

	[R12BW] = {  // N7CXI, 2000
		.mode_name         = "Robot 12 B/W",
		.Shortmode_name    = "R12BW",
		.sync_time     = 7e-3,
		.porch_time    = 0e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.291e-3,
		.line_time     = 100e-3,
		.img_wide     = 320,
		.img_high     = 120,
		.line_high   = 2,
		.color_enc     = BW },

	[R8BW] = {  // N7CXI, 2000
		.mode_name         = "Robot 8 B/W",
		.Shortmode_name    = "R8BW",
		.sync_time     = 7e-3,
		.porch_time    = 0e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.1871875e-3,
		.line_time     = 66.9e-3,
		.img_wide     = 320,
		.img_high     = 120,
		.line_high   = 2,
		.color_enc     = BW },

	[W2120] = { // KB4YZ, 1999
		.mode_name         = "Wraase SC-2 120",
		.Shortmode_name    = "W2120",
		.sync_time     = 5.5225e-3,
		.porch_time    = 0.5e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.489039081e-3,
		.line_time     = 475.530018e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = RGB },

	[W2180] = {  // N7CXI, 2000
		.mode_name         = "Wraase SC-2 180",
		.Shortmode_name    = "W2180",
		.sync_time     = 5.5225e-3,
		.porch_time    = 0.5e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.734532e-3,
		.line_time     = 711.0225e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = RGB },

	[PD50] = {  // N7CXI, 2000
		.mode_name         = "PD-50",
		.Shortmode_name    = "PD50",
		.sync_time     = 20e-3,
		.porch_time    = 2.08e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.286e-3,
		.line_time     = 388.16e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = YUV },

	[PD90] = {  // N7CXI, 2000
		.mode_name         = "PD-90",
		.Shortmode_name    = "PD90",
		.sync_time     = 20e-3,
		.porch_time    = 2.08e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.532e-3,
		.line_time     = 703.04e-3,
		.img_wide     = 320,
		.img_high     = 256,
		.line_high   = 1,
		.color_enc     = YUV },

	[PD120] = {  // N7CXI, 2000
		.mode_name         = "PD-120",
		.Shortmode_name    = "PD120",
		.sync_time     = 20e-3,
		.porch_time    = 2.08e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.19e-3,
		.line_time     = 508.48e-3,
		.img_wide     = 640,
		.img_high     = 496,
		.line_high   = 1,
		.color_enc     = YUV },

	[PD160] = {  // N7CXI, 2000
		.mode_name         = "PD-160",
		.Shortmode_name    = "PD160",
		.sync_time     = 20e-3,
		.porch_time    = 2.08e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.382e-3,
		.line_time     = 804.416e-3,
		.img_wide     = 512,
		.img_high     = 400,
		.line_high   = 1,
		.color_enc     = YUV },

	[PD180] = {  // N7CXI, 2000
		.mode_name         = "PD-180",
		.Shortmode_name    = "PD180",
		.sync_time     = 20e-3,
		.porch_time    = 2.08e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.286e-3,
		.line_time     = 754.24e-3,
		.img_wide     = 640,
		.img_high     = 496,
		.line_high   = 1,
		.color_enc     = YUV },

	[PD240] = {  // N7CXI, 2000
		.mode_name         = "PD-240",
		.Shortmode_name    = "PD240",
		.sync_time     = 20e-3,
		.porch_time    = 2.08e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.382e-3,
		.line_time     = 1000e-3,
		.img_wide     = 640,
		.img_high     = 496,
		.line_high   = 1,
		.color_enc     = YUV },

	[PD290] = {  // N7CXI, 2000
		.mode_name         = "PD-290",
		.Shortmode_name    = "PD290",
		.sync_time     = 20e-3,
		.porch_time    = 2.08e-3,
		.sep_time    = 0e-3,
		.pixel_time    = 0.286e-3,
		.line_time     = 937.28e-3,
		.img_wide     = 800,
		.img_high     = 616,
		.line_high   = 1,
		.color_enc     = YUV },

	[P3] = {  // N7CXI, 2000
		.mode_name         = "Pasokon P3",
		.Shortmode_name    = "P3",
		.sync_time     = 5.208e-3,
		.porch_time    = 1.042e-3,
		.sep_time    = 1.042e-3,
		.pixel_time    = 0.2083e-3,
		.line_time     = 409.375e-3,
		.img_wide     = 640,
		.img_high     = 496,
		.line_high   = 1,
		.color_enc     = RGB },

	[P5] = {  // N7CXI, 2000
		.mode_name         = "Pasokon P5",
		.Shortmode_name    = "P5",
		.sync_time     = 7.813e-3,
		.porch_time    = 1.563e-3,
		.sep_time    = 1.563e-3,
		.pixel_time    = 0.3125e-3,
		.line_time     = 614.065e-3,
		.img_wide     = 640,
		.img_high     = 496,
		.line_high   = 1,
		.color_enc     = RGB },

	[P7] = {  // N7CXI, 2000
		.mode_name         = "Pasokon P7",
		.Shortmode_name    = "P7",
		.sync_time     = 10.417e-3,
		.porch_time    = 2.083e-3,
		.sep_time    = 2.083e-3,
		.pixel_time    = 0.4167e-3,
		.line_time     = 818.747e-3,
		.img_wide     = 640,
		.img_high     = 496,
		.line_high   = 1,
		.color_enc     = RGB }

};

/*
 * Mapping of 7-bit VIS codes to modes
 *
 * Reference: Dave Jones KB4YZ (1998): "List of SSTV Modes with VIS Codes".
 *            http://www.tima.com/~djones/vis.txt
 *
 */

//                  0     1     2     3    4     5     6     7     8     9     A     B    C    D    E     F

uint8_t vis_map[] = { 0,    0,    R8BW, 0,   R24,  0,    R12BW,0,    R36,  0,    R24BW,0,   R72, 0,   0,    0,     // 0
	0,    0,    0,    0,   0,    0,    0,    0,    0,    0,    0,    0,   0,   0,   0,    0,     // 1
	M4,   0,    0,    0,   M3,   0,    0,    0,    M2,   0,    0,    0,   M1,  0,   0,    0,     // 2
	0,    0,    0,    0,   0,    0,    0,    W2180,S2,   0,    0,    0,   S1,  0,   0,    W2120, // 3
	0,    0,    0,    0,   0,    0,    0,    0,    0,    0,    0,    0,   SDX, 0,   0,    0,     // 4
	0,    0,    0,    0,   0,    0,    0,    0,    0,    0,    0,    0,   0,   PD50,PD290,PD120, // 5
	PD180,PD240,PD160,PD90,0,    0,    0,    0,    0,    0,    0,    0,   0,   0,   0,    0,     // 6
	0,    P3,   P5,   P7,  0,    0,    0,    0,    0,    0,    0,    0,   0,   0,   0,    0 };   // 7

//                  0     1     2     3    4     5     6     7     8     9     A     B    C    D    E     F



