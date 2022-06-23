#pragma once
#include <vector>

#include "measure.h"


class Backend {
public:
  virtual ~Backend(){}
  virtual uint numberInput() const = 0;
  virtual uint numberOutput() const = 0;
  virtual uint getSampleRate() const = 0;
  virtual bool isReady() const = 0;
  virtual vector<VD> aquisition(const vector<VD> &input) = 0;
};






