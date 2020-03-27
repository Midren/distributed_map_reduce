// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_REDUCEBASE_H
#define MAP_REDUCE_REDUCEBASE_H

#include <bits/unique_ptr.h>
#include <vector>
#include "../types/KeyValueType.h"

class ReduceBase {
public:
    virtual std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    reduce(const std::unique_ptr<KeyValueType> &key, const std::vector<std::unique_ptr<KeyValueType>> &inputs) = 0;
};


#endif //MAP_REDUCE_REDUCEBASE_H
