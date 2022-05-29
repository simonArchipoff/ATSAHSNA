#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "measure.h"

TEST_CASE("computeTHDNsimple") {
  uint t = 2*2048;
  VD v(t);
  for(uint i = 0; i < v.size(); i++){
      v[i] = i < t/2 ? 1 : -1;
    }
  ResultTHD thd = computeTHDNsimple(v,0);
  CHECK(std::abs(thd.thdRate - 0.4834) < 0.0001);
}
