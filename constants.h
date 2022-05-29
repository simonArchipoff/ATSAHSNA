#pragma once

#define APPNAME "ATSAHSNA"

typedef enum {FAUST, JACK} backend_type;

#ifndef ZEROISH
#define ZEROISH 1e-20
//if 1 is 0db, this is -460db
#endif

#define MAXFREQ 100000
#define MINFREQ 1

#define DEFAULTSR 44100
