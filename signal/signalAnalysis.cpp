#include "signalAnalysis.h"

#include <assert.h>
#include <algorithm>
#include <numeric>
#include <cmath>

#include <fstream>
#include <iomanip>

double window_sample(const enum window_type t, double s){
    double a0 = 0.53836;
    double a1 = 0.46164; // hamming parameters
    switch(t){
    case BOXCAR:
        return 1;
    break;
    case HANN:
    a0 = a1 = 0.5;
    case HAMMING:
    return a0 - a1 * cos(2*M_PI*s);
    break;
    }
}


VD window(uint size, window_type t){
    VD w(size,1.0);
    switch(t){
    case BOXCAR:
        return w;
    case HANN:
        for(uint i = 0; i < size; i++){
            w[i] = pow(sin(M_PI * static_cast<double>(i)/size),2);
        }
        return w;
    default:
        for(uint i = 0; i < size; i++){
            w[i] = window_sample(t,static_cast<double>(i)/size);
        }
        return w;
    }
}


VD correlation(const VD &v, uint start, uint size,
               const VD &k, uint kstart, uint ksize){
    assert(v.size() > start + size);
    assert(k.size() > kstart + ksize);
    VD res(size+ksize);
    for(uint i = 0; i < size; i++){
        for(uint j = 0; j < ksize; j++){
            res[i] += v[start + i + j] * k[kstart + j];
        }
    }
    return res;
}


void find_maximums(const VD & in, vector<int> & idx, vector<double> & maxs, double minimum_max){
    idx.resize(0);
    maxs.resize(0);
    if(in.size() > 1 && in[0] > in[1] && in[0] > minimum_max){ //is first element a maximum?
        idx.push_back(0);
        maxs.push_back(in[0]);
    }
    for(int i = 1; static_cast<size_t>(i) < in.size() -1 ; i++){
        if(in[i-1] < in[i] && in[i] > in[i+1] && in[i] > minimum_max){
            idx.push_back(i);
            maxs.push_back(in[i]);
        }
    }
    if(in.size() > 2 && in[in.size() - 2] < in[in.size() -1] && in[in.size() -1] > minimum_max){ //is last element a maximum?
        idx.push_back(in.size() - 1);
        maxs.push_back(in[in.size() - 1]);
    }
}


int compute_delay(const VD & out, const VD & in, int maximum_delay,int size_input){
    VD k(2*size_input);
    for(int i = 0; i < size_input; i++){
        k[i] = in[size_input - i - 1];
        k[size_input + i] = in[i];
    }
    auto corr_out = correlation(out,0,maximum_delay + size_input, k, 0, k.size());
    double m = *std::max_element(corr_out.begin(), corr_out.end());
    vector<int> idx;
    vector<double> maxs;
    find_maximums(corr_out, idx, maxs,0.99*m);
    //qDebug() << idx;
    return idx[0] + size_input;
}


//https://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
//thank you codeling
double mean(const VD &v){
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}
double stddev(const VD & v){
    double m = mean(v);
    double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0,
        [](double const & x, double const & y) { return x + y; },
        [m](double const & x, double const & y) { return (x - m)*(y - m); });
    return std::sqrt(sq_sum / v.size());
}

VD try_make_phase_continuous(const VD &v){
    vector<double> res = VD{v};
    for(uint i = 1; i < v.size() ; i++){
        auto prev = res[i-1];
        int k = static_cast<int>(std::round((prev-v[i])/360.));
        res[i] = res[i] + k * 360.;
    }
    return res;
}




