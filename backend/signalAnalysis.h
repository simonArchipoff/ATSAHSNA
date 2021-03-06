#pragma once

#include "../constants.h"

struct MeasureData{
  /*
  MeasureData(){}
  MeasureData(int nbInput, int nbOutput){
    for(int i = 0; i < nbInput; i++){
        inputs.push_back(VD());
      }
    for(int i = 0; i < nbOutput; i++){
        outputs.push_back(VD());
      }
  }*/
  int numberInput(){
    return inputs.size();
  }
  int numberOutput(){
    return outputs.size();
  }

  vector<VD> inputs;
  vector<VD> outputs;
};



//naive implementation for finding c in v
VD convolution(const VD &v, const VD&c);

double mean(const VD &v);
double stddev(const VD &v);
