/*
  ==============================================================================

    CircularBuffer.h
    Created: 26 Apr 2024 7:14:08pm
    Author:  Martin

  ==============================================================================
*/

#pragma once

template<class T, size_t size>
class CircularBuffer {
private:
    T buf[size];
    size_t oldestIndex;
public:
    CircularBuffer(const T& initValue);
    void push(const T& element);
    T& operator[](size_t index);
};

template<class T, size_t size>
inline CircularBuffer<T, size>::CircularBuffer(const T& initValue) : oldestIndex(0) {
    for (size_t i = 0; i < size; i++) {
        buf[i] = initValue;
    }
}

template<class T, size_t size>
inline void CircularBuffer<T, size>::push(const T& element) {
    this->buf[this->oldestIndex] = element;
    ++this->oldestIndex % size;
}

template<class T, size_t size>
inline T& CircularBuffer<T, size>::operator[](size_t index) {
    return this->buf[(this->oldestIndex - index - 1 + size) % size];
}
