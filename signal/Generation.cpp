#include "Generation.h"
#include <assert.h>


//I took this
//https://www.mathcore.com/resources/documents/ie_external_functions.pdf
VD chirp_angular(double w1, double w2, double duration, int sampleRate, double phase_shift){
    uint size = static_cast<uint>(duration*sampleRate);
    VD out(size);
    for(uint frame = 0; frame < size; frame++){
        double t = frame * 1/static_cast<double>(sampleRate);
        out[frame] = cos(phase_shift + (w1 * t + (w2 - w1) * t * t / (2*duration)));
    }
    return out;
}

VD chirp(double f1, double f2, double duration, uint sampleRate){
    return chirp_angular(2*M_PI * f1,2*M_PI*f2,duration,sampleRate,M_PI/2.);
}

VCD chirp_complex(double f1,double f2, double duration, unsigned int sampleRate){
    VD r = chirp_angular(2*M_PI * f1,2*M_PI*f2,duration,sampleRate,M_PI/2.);
    VD i = chirp_angular(2*M_PI * f1,2*M_PI*f2,duration,sampleRate,M_PI);
    assert(r.size() == i.size());
    VCD c(r.size(),0);
    for(uint j = 0; j < c.size(); j++){
        c[j] = std::complex<double>(r[j],i[j]);
    }
    assert(r.size() == c.size());
    return c;
}

VD sinusoid(double f, double duration, unsigned int sampleRate){
    return chirp(f,f,duration,sampleRate);
}

vector<double> impulse(double freqMin, double duration, uint sampleRate){
    assert(freqMin > 0 && freqMin < sampleRate / 2);
    int dur = std::max(duration*sampleRate,sampleRate / freqMin);
    vector<double> out(dur,0.0);
    out[0] = 1;//(double)duration;
    return out;
}
