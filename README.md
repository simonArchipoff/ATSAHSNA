# ATSAHSNA

This project aims to be a free/libre software to perform accoustical measures, some kind of REW without the "ew",
It's licenced under GPLv3.

What's already implemented:
- two backend (faust and jackd)
- impulse response and THD spectrum and THD+N computation
- a very poor UI, sorry

Soon :
- something to deal with the latency more easily
- visualization in temporal domain (see point above)
- wavelet waterfall measures
- export measurements in a bunch of standards files
- better UI
- dealing with several input/output (to measure two channels at once for instance)
- simpler code (I am tâtonning)

Later :
- Implementing what's needed to measures an impedance and/or do computation with inputs
- a full laplace transform backend, with physical measures (?)
- Some kind of knob-turning algorithm to create model of a measured function (eg, turning a faust equalizer's knobs to flatten a response).

I'll try not to spread qt things all around so that it'll be easy to make a script friendly measure tool.
I hope I'll be able to propose a headless system able to compute data such as THD against power or THD against freqency

Dependancy:
* Qt
* QCoro
* FFTW
* Faust
* Jack
* doctest 
* moodycamel lockfree queue (already in the repository)


