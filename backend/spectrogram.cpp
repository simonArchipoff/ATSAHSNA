#include "spectrogram.h"

#include "measure.h"
//#include <wavelib.h>



#include <iostream>
#include <vector>
#include <math.h>

#include <fcwt.h>




SpectrogramData spectrogram(const std::vector<double> &data,
                            int nb_octave, int resolution, uint sampleRate){
  struct SpectrogramData res;


  int n = data.size(); //signal length
//  float fs = sampleRate; //sampling frequency

  //3000 scales spread over 5 octaves
  const int noctaves = 5; //nb_octave;
  const int nsuboctaves = 600;

  //input: n real numbers
  std::vector<float> sig(n);

  //output: n x scales complex numbers
  std::vector<float> tfm(n*noctaves*nsuboctaves*2);

  for(int i = 0 ; i < n; i++){
      sig[i] = static_cast<float>(data[i]);
    }

  //Arguments cwt:
  //input     - floating pointer to input array
  //length    - integer signal length
  //output    - floating pointer to output array
  //startoct  - scale range begin (2^startoct)
  //endoct    - scale range end (2^endoct)
  //suboct    - exponential subdivisions of each octave
  //sigma     - parameter to control time-frequency precision
  //nthreads  - number of threads to use
  //optplans  - use FFTW optimization plans if true

  fcwt::cwt(&sig[0]
      , n
      , &tfm[0]
      , 1
      , noctaves
      , nsuboctaves
      , 2*M_PI/*I dont understand this parameter, I just cargo cult*/
      , 8
      , false);

  res.data.resize(tfm.size()/2);
  for(uint p = 0; p < tfm.size(); p += 2){
      float r = tfm[p]
          , i = tfm[p+1];
      res.data[p] = sqrt(r*r + i*i);
    }

  /*
  cwt_object wt;
  wt = cwt_init("morlet", 4, data.size(), 1.0/sampleRate , scale_freq);
  setCWTScales(wt, (1.0*freq_max)/sampleRate, 1.0/scale_freq, "pow",2);
  for(int i = 0; i < scale_freq; i++){
      qDebug() << wt->scale[i];
  }
  cwt(wt, data.data());



  res.data.resize(scale_freq);

  for(uint s = 0; s < scale_freq; s++){
      res.data[s].resize(data.size());
      for (uint i = 0; i < data.size(); ++i) {
          auto c = wt->output[s * scale_freq + i];
          res.data[s][i] = sqrt(c.re * c.re + c.im * c.im);
        }
    }

  cwt_summary(wt);

  cwt_free(wt);
  */
  return res;
}



#if 0




inp = (double*)malloc(sizeof(double)* N);
oup = (double*)malloc(sizeof(double)* N);

for (i = 0; i < N; ++i) {
	inp[i] = temp[i] ;
}



printf("\n MEAN %g \n", wt->smean);

mn = 0.0;

for (i = 0; i < N; ++i) {
	mn += sqrt(wt->output[i].re * wt->output[i].re + wt->output[i].im * wt->output[i].im);
}



printf("\n abs mean %g \n", mn / N);

printf("\n\n");
printf("Let CWT w = w(j, n/2 - 1) where n = %d\n\n", N);
nd = N/2 - 1;

printf("%-15s%-15s%-15s%-15s \n","j","Scale","Period","ABS(w)^2");
for(k = 0; k < wt->J;++k) {
	iter = nd + k * N;
	printf("%-15d%-15lf%-15lf%-15lf \n",k,wt->scale[k],wt->period[k],
	wt->output[iter].re * wt->output[iter].re + wt->output[iter].im * wt->output[iter].im);
}

icwt(wt, oup);

num = den = recon_var = recon_mean = 0.0;
printf("\n\n");
printf("Signal Reconstruction\n");
printf("%-15s%-15s%-15s \n","i","Input(i)","Output(i)");

for (i = N - 10; i < N; ++i) {
	printf("%-15d%-15lf%-15lf \n", i,inp[i] , oup[i]);
}

for (i = 0; i < N; ++i) {
	//printf("%g %g \n", oup[i] ,inp[i] - wt->smean);
	td = inp[i] ;
	tn = oup[i] - td;
	num += (tn * tn);
	den += (td * td);
	recon_mean += oup[i];
}

recon_var = sqrt(num / N);
recon_mean /= N;

printf("\nRMS Error %g \n", sqrt(num) / sqrt(den));
printf("\nVariance %g \n", recon_var);
printf("\nMean %g \n", recon_mean);

free(inp);
free(oup);

#endif
