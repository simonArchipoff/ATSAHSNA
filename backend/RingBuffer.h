#pragma once
#include <vector>
#include <cassert>

typedef unsigned int uint;
template<typename T>
class RingBuffer
{
public:
    RingBuffer():begin(0),end(0),size(0){}
    RingBuffer(int size):RingBuffer(){
        reset(size);
    }
    void reset(int size){
        buff.resize(size);
    }

    std::vector<T> read(uint size) const{
        assert(size <= available());
        std::vector<T> res;
        res.resize(size,42);
        if(buff.size() - begin >= size){
            std::copy(buff.cbegin() + begin, buff.cbegin() + begin + size,res.begin());
        } else {
            int tmp = buff.size() - begin;
            assert(tmp < int(size));
            std::copy(buff.cbegin() + begin, buff.end(), res.begin());
            std::copy(buff.cbegin(), buff.cbegin() + (size - tmp)  , res.begin() + tmp);
        }
        return res;
    }

    void pop(uint size){
        assert(size <= available());
        begin += size;
        begin %= buff.size();
        this->size -= size;
    }
    void write(const std::vector<T> & b){
        write(b.data(), b.size());
    }

    void write(const T * b, uint size){
        if(freespace() < size){
            pop(size - freespace());
        }
        if(size <= buff.size() - end){
            std::copy(b,b+size,buff.begin() + end);
            end += size;
            end %= buff.size();
        } else {
            int tmp = buff.size() - end;
            std::copy(b,b + tmp, buff.begin() + end);
            std::copy(b + tmp, b+size,buff.begin());
            end = size - tmp;
        }
        this->size += size;
    }
    uint available() const {
        return size;
    }
    uint freespace() const {
        return buff.size() - size;
    }

    void reset(){
        begin = end = 0;
    }

private:
    std::vector<T> buff;
    uint begin,end,size;
};
