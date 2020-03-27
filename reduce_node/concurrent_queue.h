// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_CONCURRENT_QUEUE_H
#define MAP_REDUCE_CONCURRENT_QUEUE_H

#include<deque>
#include<condition_variable>
#include<mutex>

template<class T>
class ConcurrentQueue {
public:
    ConcurrentQueue() = default;

    void push(const T &element);

    void push(T &&element);

    T pop();

private:
    std::deque<T> data;
    std::condition_variable queue_check;
    std::mutex deque_mutex;
};

#include "concurrent_queue.cpp"

#endif //MAP_REDUCE_CONCURRENT_QUEUE_H
