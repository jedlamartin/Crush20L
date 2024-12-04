#pragma once

#include "JuceHeader.h"
#include <mutex>

template<class T>
class ParameterQueue {
private:
    T parameterSet;
    bool empty;
    std::mutex mutex;
public:
    ParameterQueue() :parameterSet(), empty(true) {}
    bool read_and_pop(T& value) {
        if (mutex.try_lock()) {
            value = parameterSet;
            bool tmp = empty;
            empty = true;
            mutex.unlock();
            return !tmp;
        }
        else {
            return false;
        }
        
    }
    void push(const T& value) {
        std::lock_guard<std::mutex> guard(mutex);
        parameterSet = value;
        empty = false;
    }
};