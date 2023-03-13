#pragma once

#include <vector>
#include <complex>

#define APPNAME "ATSAHSNA"
#define APPVERS "0.0.1"

typedef enum {UNDEFSIGNAL, CHIRP, IMPULSE} signal_gen_type;
typedef enum {UNDEFBACKEND, FAUST, JACK} backend_type;
typedef enum {UNDEFCOMPUTATION, RESPONSE, HARMONICS, SPECTROGRAM} computation_type;


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
