#ifndef MAP_REDUCE_REDUCEBASE_H
#define MAP_REDUCE_REDUCEBASE_H

#include <bits/unique_ptr.h>
#include <vector>
#include "ValueType.h"

class ReduceBase {
public:
    virtual std::unique_ptr<ValueType> reduce(const std::vector<std::unique_ptr<ValueType>> &inputs) = 0;
};


#endif //MAP_REDUCE_REDUCEBASE_H
