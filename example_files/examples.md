# Source:
These examples have been produced with Adplay for DOS 1.6, running under DOSBox-X version 2023.10.06 SDL2, with the following options changed:
```
[mixer]
sample_accurate = true
rate            = 49716

[sblaster]
oplrate         = 49716
```

The file played was `loudness.lds` from the game Tyrian. It was chosen because it's a small file.
Recording of DRO files can be toggled from the command line:

```bash
# Assumes you have adplay.exe and loudness.lds in your working directory
dosbox-x --fastlaunch --nomenu --time-limit 10 --exit -c "mount c ." -c "c:" -c "cls" -c "dx-capture /a /o adplay.exe -q loudness.lds"
```

## Extra files need to reproduce:
* [`dosbox-x`](https://dosbox-x.com/)
* [`adplay`](https://github.com/adplug/adplay-dos/releases)
* [`loudness.lds`](https://github.com/adplug/adplug/raw/master/test/testmus/loudness.lds)


