slowrx
======
Slowrx is an excellent SSTV decoder for Linux created by Oona Räisänen (OH2EIQ [at] sral.fi).<br>
http://windytan.github.io/slowrx/<br>
Unfortunately it uses a GTK GUI and reads audio from a soundcard and I wanted to convert prior WAV recordings on a headless Raspberry Pi.<br>

slowrx-cli is a hack that runs on the command line (no GUI), reads a 44100 samples per second WAV file, and writes a BMP. I don't think I've broken anything but, so far, I have not tested all the SSTV modes Oona coded for.

Requirements
------------
* Linux (I'm using a Raspberry Pi)
* FFTW 3 (`libfftw3-dev`)

Compiling
---------
`make`

Running
-------
`./slowrx-cli {opts} filename`<br>
`  -a           adaptive (default)`<br>
`  -na          not adaptive`<br>
`  -o filename  output bitmap filename (default result.bmp)`<br>
`  -v           verbose output`<br>
`  -r float     override WAV sample rate, Hz`<br>
`  -s int       sync. adjustment (default 0.0)`<br>
`  -h           show this information`<br>
