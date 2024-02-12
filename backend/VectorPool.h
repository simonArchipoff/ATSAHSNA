#pragma once
#include <cstdlib>
#include <vector>
#include <AudioIO.h>


template<typename T>
class VectorPool{
public:
    VectorPool(int number,int size):pool(number),size(size){
        for(auto & i : pool){
            i = new T[size];
        }
    }
    ~VectorPool(){
        for(auto i : pool){
            delete[] i;
        }
    }
    VectorCStyle<T> getVector(){
        for(auto & i : pool){
            if(i){
                auto tmp = i;
                i = nullptr;
                VectorCStyle<T> res;
                res.v = tmp;
                res.v_size = size;
                return res;
            }
        }
        abort();
    }
    void putVectorBack(VectorCStyle<T> &v){
        for(auto & i : pool){
            if(!i){
                i = v.data();
                return;
            }
        }
    }
private:
    std::vector<T *> pool;
    int size;
};
