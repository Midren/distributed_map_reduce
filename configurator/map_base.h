// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_MAP_BASE_H
#define MAP_REDUCE_MAP_BASE_H

#include "../types/KeyValueType.h"
#include <bits/unique_ptr.h>

namespace map_reduce {
    class map_base {
    public:
        //Not thread-safe. Use clone method before if you need to run map simultaneously in different threads
        std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
        run_map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) {
            res.clear();
            this->map(key, value);
            return std::move(res);
        }

        void emit(std::unique_ptr<KeyValueType> key, std::unique_ptr<KeyValueType> value) {
            res.emplace_back(std::move(key), std::move(value));
        }

        virtual std::unique_ptr<map_base> clone() = 0;

    protected:
        virtual void map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) = 0;

    private:
        std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> res;
    };

}

#endif //MAP_REDUCE_MAP_BASE_H
