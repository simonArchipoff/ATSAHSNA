#include "signalHarmonics.h"

#include <numeric>
#include <algorithm>

/*
double mean(const std::vector<double> &v){
  return std::reduce(std::execution::par_unseq,v.begin(),v.end())/static_cast<double>(v.size());
}

double std_dev(const std::vector<double> &v){
  auto m=mean(v);
  return 0;
}
*/

ResultTHD computeTHDNsimple(const ParamTHD p, const VD&signal, int sampleRate){
  assert(signal.size() > 1);

  VCD signalfft = computeDFT(signal);
  signalfft[0] = signalfft[1];
  uint smin = p.freqMin *  p.duration;
  uint smax = std::min<uint>(p.freqMax * p.duration, signal.size()/2);
  //find fundamental (let's suppose it's the maximum)
  uint imax=0;
  double eh1 = 0;
  for(uint i = 0; i < signal.size()/2; i++){
      if(abs(signalfft[i]) > eh1){
          eh1 = abs(signalfft[i]);
          imax = i;
        }
  }

  double e_tot_wo_h1 = 0;
  for(uint i = smin; i < smax ; i++){
      double tmp = real(signalfft[i] * conj(signalfft[i]));
      if(i != imax){
          e_tot_wo_h1 += tmp;
        }
    }

  for(uint i = 0; i < signalfft.size(); i++){
      signalfft[i] /= eh1;
    }


  /*
  for(uint i = 0; i < smin; i++){
      signalfft[i]=1;
    }
  for(uint i = smax; i < signalfft.size(); i++)
    signalfft[i]=1;
*/
  return ResultTHD {
      .harmonicSpectrum = FDF(signalfft,sampleRate),
      .thdRate = sqrt(e_tot_wo_h1) / eh1,
      .params = p,
      .raw_data = MeasureData { .inputs = vector<VD>({}),
                                .outputs = vector({signal})}
    };
  //return sqrt(e_tot_wo_h1) / sqrt(e_tot_wo_h1 + eh1 * eh1);
}






//the slice data type and the get_harmonic function locate the harmonics
// the function try to find the local maximum around its parameter f
//         /\
//        /  \
//   \_  /    \    /\
//     \/      \/\/
//begin^    end^
//level is the integral between begin and end included
struct slice {
  uint begin;
  uint end;
  double level;
};

slice get_harmonic(const uint f, const vector<double>&v){
  assert(f < v.size());
  struct slice s;

  uint f_l = f, f_r=f;
  while(f_l > 1 && v[f_l - 1] > v[f_l])
    f_l--;
  while(f_r < v.size() -1 && v[f_r + 1] > v[f_r])
    f_r++;
  if(v[f_l] > v[f_r])
    s = {f_l,f_l,v[f_l]};
  else
    s = {f_r,f_r,v[f_r]};


  // find local minimum left
  while(s.begin >= 1 && v[s.begin - 1] <= v[s.begin]){
      s.level += v[s.begin - 1];
      s.begin--;
    }
  //find local minimum right
  while(s.end < v.size() -1 &&  v[s.end] >= v[s.end + 1]){
      s.level += v[s.end + 1];
      s.end++;
    }
  return s;
}

vector<slice> find_harmonics(const vector<double> & v,uint fundamental){
  vector<slice> res;
  for(auto i = 1; i*fundamental < v.size(); i++){
      res.push_back(get_harmonic(i*fundamental,v));
    }
  return res;
}
vector<slice> find_harmonics(const vector<double> &v){
  auto f = std::distance(v.begin(),std::max_element(v.begin(),v.end()));
  return find_harmonics(v,f);
}

double thd_ieee(const vector<slice> & slices){
  double a = 0, b = 0;
  vector<slice>::const_iterator  it;
  for(it = slices.begin(), it++ ; it != slices.end(); it++){
      a += it->level * it->level;
    }
  b = slices[0].level;
  return 100*sqrt(a)/b;
}

template<typename T>
std::pair<T,T> sum_pair(const std::pair<T,T> &a
                        ,const std::pair<T,T> &b){
  return std::make_pair(a.first + b.first, a.second + b.second);
}

vector<std::pair<double,double>> decimation_log(const vector<std::pair<double,double>> & v, uint nb_points){
  auto b = log(v.size())/nb_points;
  vector<std::pair<double,double>> res;
  res.resize(nb_points);

  for(uint i = 0; i < nb_points; i++){
      const uint imin = static_cast<uint>(exp(b*i));
      const uint imax = static_cast<uint>(exp(b*(i+1)));
      auto p = reduce(v.begin() + imin, v.begin() + imax, std::make_pair(0,0), sum_pair<double>);
      p.first /= imax - imin;
      p.second /= imax - imin;
      res[i] = p;
    }

  return res;
}

/*
int main(int argc, char ** argv){
    auto v = vector<double>({0,1,0,2,1,0.2,0.1,0.2,0.1,5,4,2,0.1,0.2,0.1,0.2,});

    for(auto s : find_harmonics(v,3)){
        qDebug() << s.begin << s.end << s.level;
    }


    return 0;
}
*/




/*
struct THD {
  FDF harmonicSpectrum;
  double thdRate;
  vector<double> harmonicsLevel;
  vector<double> harmonicsPhases;
};
*/

//https://dsp.stackexchange.com/questions/41291/calculating-the-phase-shift-between-two-signals-based-on-samples
double phaseShift(const VD &a, const VD &b){
  assert(a.size() == b.size());
  //compute norms and dot product of the vectors
  double na = 0,nb=0, dp=0;
  for(uint i = 0; i < a.size(); i++){
      na += a[i] * a[i];
      nb += b[i] * b[i];
      dp += a[i] * b[i];
    }
  na = sqrt(na);
  nb = sqrt(nb);
  return acos(dp/(na*nb));
}
