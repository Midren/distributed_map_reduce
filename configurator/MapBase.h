// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_MAPBASE_H
#define MAP_REDUCE_MAPBASE_H

#include "../types/KeyValueType.h"
#include <bits/unique_ptr.h>

namespace map_reduce {
    class MapBase {
    public:
        virtual std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
        map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) = 0;
    };
}

#endif //MAP_REDUCE_MAPBASE_H
