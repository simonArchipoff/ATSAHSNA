#pragma once
#include <vector>
#include <cassert>

template<typename T>
class RingBuffer
{
public:
    RingBuffer():size(0),begin(0),end(0){}
    RingBuffer(int size):RingBuffer(){
        reset(size);
    }
    void reset(int size){
        buff.resize(size);
    }

    std::vector<T> read(int size) const{
        assert(size <= available());
        std::vector<T> res;
        res.resize(size,42);
        if(buff.size() - begin >= size){
            std::copy(buff.cbegin() + begin, buff.cbegin() + begin + size,res.begin());
        } else {
            int tmp = buff.size() - begin;
            assert(tmp < size);
            std::copy(buff.cbegin() + begin, buff.end(), res.begin());
            std::copy(buff.cbegin(), buff.cbegin() + (size - tmp)  , res.begin() + tmp);
        }
        return res;
    }

    void pop(int size){
        assert(size <= available());
        begin += size;
        begin %= buff.size();
        this->size -= size;
    }
    void write(const std::vector<T> &b){
        assert(freespace() >= b.size());
        if(b.size() <= buff.size() - end){
            std::copy(b.begin(),b.end(),buff.begin() + end);
            end += b.size();
            end %= buff.size();
        } else {
            int tmp = buff.size() - end;
            std::copy(b.begin(),b.begin() + tmp, buff.begin() + end);
            std::copy(b.begin() + tmp, b.end(),buff.begin());
            end = b.size() - tmp;
        }
        this->size += b.size();
    }
    int available() const {
        return size;
    }
    int freespace() const {
        return buff.size() - size;
    }

private:
    std::vector<T> buff;
    int begin,end,size;
};
