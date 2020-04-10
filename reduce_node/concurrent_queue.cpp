// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "concurrent_queue.h"

namespace map_reduce {

    template<class T, template<class> class Container>
    void ConcurrentQueue<T, Container>::push(const T &element) {
        std::unique_lock<std::mutex> locker(container_mutex);
        data.push_back(element);
        locker.unlock();
        queue_check.notify_one();
    }

    template<class T, template<class> class Container>
    void ConcurrentQueue<T, Container>::push(T &&element) {
        std::unique_lock<std::mutex> locker(container_mutex);
        data.push_back(std::move(element));
        locker.unlock();
        queue_check.notify_one();
    }

    template<class T, template<class> class Container>
    T ConcurrentQueue<T, Container>::pop() {
        std::unique_lock<std::mutex> locker(container_mutex);
        queue_check.wait(locker, [this] { return !data.empty(); });
        T a = std::move(data.front());
        data.pop_front();
        return a;
    }
}
