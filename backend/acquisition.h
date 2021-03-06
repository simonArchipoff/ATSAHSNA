#pragma once
#include "backend.h"

#include "signalResponse.h"
#include "signalHarmonics.h"
#include "signalSpectrogram.h"



vector<struct ResultResponse>    compute_response   (Backend *b, const struct ParamResponse p);
vector<struct ResultTHD>         compute_distortion (Backend *b, const struct ParamTHD p);
vector<struct ResultSpectrogram> compute_spectrogram(Backend *b, const struct ParamSpectrogram p);
