# ATSAHSNA

This project aims to be a free/libre software to perform accoustical measures, some kind of REW without the "ew",
It's licenced under GPLv3. For now it's under heavy development, you can play with it but please dont build something with it.

What's already implemented:
- two backend (faust and jackd)
- impulse response, harmonic spectrum and THD+N computation (in temporal domain as well), it works great for the faust backend only, it needs more work for jack
- wavelet waterfall measures
- a very poor UI, sorry

Soon :
- sweeps instead of diracs for better results
- something to deal with latency more easily
- export measurements in a bunch of standards files
- better UI
- less franglish and proper translations
- dealing with several input/output (to measure two channels at once for instance)
- having several measure displayed at once
- simpler code (I am tâtonning)

Later :
- Implementing what's needed to measures an impedance and/or do computation with inputs
- a full laplace transform backend, with physical measures (?)
- Some kind of knob-turning algorithm to create model of a measured function (eg, turning a faust equalizer's knobs to flatten a response).

Sooner or later :
- I hope I'll be able to propose a headless system able to compute data such as THD against power or THD against freqency


to install and try :
```
git submodule init
git submodule update
cmake -S . -B build && make -j 8 -C build
./build/ATSAHSNA 
```

Dependancy:
* Qt
* FFTW
* fCWT (a wavelet lib, in a git submodule, this lib also depends on FFTW in itself)
* Faust
* Jack
* moodycamel lockfree queue (already in the repository)
