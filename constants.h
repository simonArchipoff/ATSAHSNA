#pragma once

#include <vector>
#include <complex>

#define APPNAME "ATSAHSNA"

typedef enum {CHIRP, IMPULSE} signal_gen_type;


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
