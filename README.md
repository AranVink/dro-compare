# dro-compare
Compare Dosbox DRO v2 files

Quick and easy CLI tool to compare 2 Dosbox DRO v2 files.

## Example usage
```bash
dro_compare example_files/0.dro example_files/1.dro
```
This tool compares the headers, as well as any commands send to the registers. It has optional handling of delays (see below). If no arguments are specified it goes through the file, matching every command and value send to the registers.

### Remove delays
Optionally it can discard any delays in the songs. This is usefull if you want to compare 2 different recordings, since small millisecond differences can occur in delays between the recording. 

```bash
dro_compare -r example_files/0.dro example_files/1.dro
```

### Match only total sum of delays
This will add up any delays found in the recording, and validate if the total match. For example `2x1ms` delay would still match a `1x2ms` delay. Note that this does not guarantee 100% correctness of timing within the recording, just that the **same amount** of delays are there across the song.

```bash
dro_compare -m example_files/0.dro example_files/1.dro
```

## Compiling
Only Linux, GCC and some standard C libraries are required. Simply run:
```bash
make install
```
and it should be installed in /usr/local/bin

## FAQ
### Why not use a simple file compare tool?
Small millisecond delays show up in the file as command/value pairs, but not always at the same location or exact duration, so comparing them is hard. The exact cause of these delays differences is hard to pinpoint, but is assumed to be mainly because Dosbox and derivatives are not cycle-accurate ([source](https://dosbox-x.com/wiki/Guide%3ACPU-settings-in-DOSBox%E2%80%90X#_is_dosbox_x_cycle_accurate)).

### Why not compare the 2 audio output streams?
Dosbox is not 100% accurate in it's emulation, so small discrepancies can sneak into an audio recording, throwing off most comparison techniques. For example taking recording `0.wav` and `1.wav` from the `example_files` folder and comparing them shows minute differences. You won't hear them even with the best audio equipment, but they are there, sometimes differences as small as a few samples/milliseconds. This is not just from the start of the actual recording, which can vary in the bit of silence in between, but also during the song these small differences are observed.

I've tried many different approaches to try and match the audio, with *NO* success:
#### [Audacity](https://www.audacityteam.org/)
Importing 2 audio waveforms, truncating the silence at the start, inverting the second waveform and then listening to the result. In a 100% duplicate recording the net result should be complete silence, since these 2 waveforms cancel each other out.

#### [DeltaWave Audio Null Comparator](https://deltaw.org/)
This tool can compare audio , correct for level differences, phase correction, clock drift correction etc. Also this tool reports the recording are not the same.
