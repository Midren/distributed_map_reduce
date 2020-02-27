#ifndef MAP_REDUCE_REDUCEBASE_H
#define MAP_REDUCE_REDUCEBASE_H

#include <bits/unique_ptr.h>
#include <vector>
#include "types/KeyValueType.h"

class ReduceBase {
public:
    virtual std::unique_ptr<KeyValueType> reduce(const std::unique_ptr<KeyValueType> &key, const std::vector<std::unique_ptr<KeyValueType>> &inputs) = 0;
};


#endif //MAP_REDUCE_REDUCEBASE_H
