#pragma once

#include <vector>
#include <complex>
#include <algorithm>

#define APPNAME "ATSAHSNA"
#define APPVERS "0.0.1"

enum signal_gen_type {UNDEFSIGNAL, CHIRP, IMPULSE};
enum backend_type {UNDEFBACKEND, FAUST, JACK};
enum computation_type {UNDEFCOMPUTATION, RESPONSE, HARMONICS, SPECTROGRAM};

using std::vector;
using std::complex;

typedef vector<complex<double>> VCD;
typedef vector<double> VD;

#ifndef ZEROISH
#define ZEROISH 1e-20
//if 1 is 0db, this is -460db
#endif

#define MAXFREQ 100000
#define MINFREQ 1

#define DEFAULTSR 44100


