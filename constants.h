#pragma once

#include <vector>
#include <complex>
#include <algorithm>

#define APPNAME "ATSAHSNA"
#define APPVERS "0.0.1"


using std::vector;
using std::complex;

typedef vector<complex<double>> VCD;
typedef vector<complex<float>>  VCF;
typedef vector<double> VD;
typedef vector<float>  VF;


#ifndef ZEROISH
#define ZEROISH 1e-20
//if 1 is 0db, this is -460db
#endif

#define MAXFREQ 20000
#define MINFREQ 10

#define DEFAULTSR 48000

