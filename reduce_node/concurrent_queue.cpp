// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "concurrent_queue.h"

namespace map_reduce {

    template<class T>
    void ConcurrentQueue<T>::push(const T &element) {
        std::unique_lock<std::mutex> locker(deque_mutex);
        data.push_back(element);
        locker.unlock();
        queue_check.notify_one();
    }

    template<class T>
    void ConcurrentQueue<T>::push(T &&element) {
        std::unique_lock<std::mutex> locker(deque_mutex);
        data.push_back(std::move(element));
        locker.unlock();
        queue_check.notify_one();
    }

    template<class T>
    T ConcurrentQueue<T>::pop() {
        std::unique_lock<std::mutex> locker(deque_mutex);
        queue_check.wait(locker, [this] { return !data.empty(); });
        T a = std::move(data.front());
        data.pop_front();
        return a;
    }
}
