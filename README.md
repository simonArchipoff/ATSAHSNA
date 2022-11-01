# ATSAHSNA

This project aims to be a free/libre software to perform accoustical measures.

It's licenced under GPLv3. For now it's under heavy development, you can play with it but please dont build something with it yet.

What's already implemented:
- two backend (faust and jackd)
- impulse response
- harmonic spectrum, THD computation (THD, THD+N, SNR)
- wavelet waterfall measures
- temporal response
- a very poor GUI, sorry

Soon :
Regarding UI :
- better UI
- reactive interface (temporary bug)
- export measurements in a bunch of standards files
- handling of several measures at once
- less franglish and proper translations
- having several measure displayed at once

Regarding IO and data acquisition :
- dealing with several input/output (to measure two channels at once for instance)
  - mixing soundcards (for instance to test the input of one against the output of an other)
- simpler code (I am tâtonning)
- introduce arbitrary correction from input (like the inverse of a microphone response)
- more backends (C, C++, Python, soundfile, something more standard like pulseaudio) 
- some tests for more trust in the computations.

Regarding treatment and analysis :
- something to smooth data (amplitude and phase)
- something to deal with noisy measures (a wiener filter (?))

Later :
- Implementing what's needed to measures an impedance and/or do computation with inputs
- a full laplace transform backend, with physical measures (?)
- Some kind of knob-turning algorithm to create model of a measured function (eg, turning a faust equalizer's knobs to flatten a response).

Sooner or later :
- A headless system to compute data such as THD against power or THD against frequency using external tools


to install and try (should works on Arch Linux at least) :
```
git submodule init
git submodule update
cmake -S . -B build && make -j 8 -C build
./build/ATSAHSNA 
```

Dependancy:
* Qt
* FFTW
* fCWT (a continuous wavelet lib, in a git submodule, this lib itself also depends on FFTW)
* Faust
* Jack
* moodycamel lockfree queue (already in the repository)

When I started this project, I didn't knew that one : https://github.com/mincequi/qLouder, you should check it out as well
