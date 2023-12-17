#pragma once
#include <vector>



template<typename T>
class VectorPool{
public:
  VectorPool():pool(){}
  void init(int number, int size){
    for(auto i : pool){
      delete[] i;
    }
    pool.resize(number);
    for(auto & i : pool){
      i=new T[size];
    }
  }
  ~VectorPool(){
    for(auto i : pool){
      delete[] i;
    }
  }
  T * getVector(){
    for(auto & i : pool){
      if(i){
	auto tmp = i;
	i = nullptr;
	return tmp;
      }
    }
    return nullptr;
  }
  void putVectorBack(T *v){
    for(auto & i : pool){
      if(!i){
	i = v;
	return;
      }
    }
  }
private:
  std::vector<T *> pool;
};
