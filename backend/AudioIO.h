#pragma once
#include <cassert>
#include <sys/types.h>

const uint max_channels = 8;

//class for encapsulating raw C style vectors
template<typename T>
class VectorCStyle{
public:
  VectorCStyle():v_size(0),v(nullptr){}
  VectorCStyle(uint size, T *v):v_size(size),v(v){}
  VectorCStyle(const VectorCStyle<T> &i):VectorCStyle<T>(i.v_size,i.v){
  }
  T& operator[](uint i){
    assert(i < v_size);
    return v[i];
  }
/*
  VectorCStyle<T> & operator=(const VectorCStyle<T> &i){
    VectorCStyle<T> s(i);
    return s;
  }
*/
  bool operator!=(const VectorCStyle<T> & a){
    return !(a.v_size == v_size && a.v == v);
  }
  T* data(){
    return v;
  }
  uint size() const{
    return v_size;
  }
  T*begin(){
    return v;
  }
  T*end(){
    return v + v_size;
  }
  uint v_size;
  T * v;
};


template<typename T, uint max_channels=max_channels>
class AudioIO{
public:
  AudioIO():number_channels(){};
  AudioIO(uint size, uint nb_channels, T ** v):number_channels(nb_channels){
    for(uint i = 0; i < number_channels; i++){
      channels[i] = VectorCStyle(size,v[i]);
    }
  }
  /*
  template<typename C>
  AudioIO(C v){
    number_channels = v.size();
    for(uint i = 0; i < number_channels; i++){
      channels[i] = VectorCStyle(v[i].size(),v[i].data());
    }
  }
*/
  void addChannel(uint size, T*v){
    channels[number_channels++] = VectorCStyle(size,v);
    assert(number_channels <= max_channels);
  }

  uint getNumberChannels(){
    return number_channels;
  }
  VectorCStyle<T> & operator[](uint i) {
    assert(i < number_channels);
    return channels[i];
  }

  const VectorCStyle<T> & operator[](uint i) const {
    assert(i < number_channels);
    return channels[i];
  }

  VectorCStyle<T> * begin() {
    return &channels[0];
  }
  VectorCStyle<T> * end() {
    return &channels[number_channels];
  }

  int size() const{
    return number_channels;
  }

private:
  VectorCStyle<T> channels[max_channels];
  uint number_channels;
};
