#pragma once
#include <backend.h>
#include <map>
#include <signalResponse.h>
#include <signalHarmonics.h>


template<class C>
class Acquisition
{
public:
    Acquisition(Backend *);

    std::string compute(typename C::Param p, std::string name=C::name);

    std::map<std::string,vector<typename C::Result>> result;
    Backend * backend;
};



