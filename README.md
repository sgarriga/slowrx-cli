slowrx
======
Slowrx is an excellent SSTV decoder for Linux created by Oona Räisänen (OH2EIQ [at] sral.fi).<br>
http://windytan.github.io/slowrx/<br>
Unfortunately it uses a GTK GUI and reads audio from a soundcard, while I wanted to convert prior WAV recordings on a headless Raspberry Pi.<br>

slowrx-cli is a hack that runs on the command line (no GUI), reads a 44100 Hz WAV file, and writes a BMP. I don't think I've broken anything but I have not tested all the SSTV modes Oona coded for. I *have* done a lot of refactoring (on my own and with Github Copilot), added a couple of extra modes and added an option to use the Goertzel Algorithm to detect the 1200 Hz sync tone.

Requirements
------------
* Linux (Raspbian and Ubuntu work for me, your mileage may vary)
* FFTW 3 (`libfftw3-dev`)
* gcc

Compiling
---------
`make`

Running
-------
`./slowrx-cli {opts} filename`<br>
`  -a           adaptive (default)`<br>
`  -mx          mirror X axis`<br>
`  -my          mirror Y axis`<br>
`  -na          not adaptive`<br>
`  -o filename  output bitmap filename (default result.bmp)`<br>
`  -v           verbose output`<br>
`  -r float     override WAV sample rate, Hz (default 44100)`<br>
`  -s int       sync. adjustment (default 0.0)`<br>
`  -g           use Goertzel algorithm for sync detection`<br>
`  -h           show this information`<br>
`  -modes       list supported SSTV modes`<br>

Robot (8-BW, 12-BW, 24-BW, 24, 36, 72), Martin (1, 2) & Scottie (1, 2, DX) test recordings were obtained from http://www.sigidwiki.com in MP3 format and converted to WAV with ffmpeg.
PD180 & PD120 WAVs came from https://spacecomms.wordpress.com/iss-sstv-audio-recordings/.
Wrasse SC2 180 WAV came from https://www.youtube.com/watch?v=KUojjnTkmV0 (Ardi Tan).

Others from Kayra Gündoğar ÇETİN YouTube playlist https://www.youtube.com/@serebryanyy_volk

Wraase SC-2 60 & 120 still need a lot of work
Wraase SC-2 30 & Robot 12 are completely untested
