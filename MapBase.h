#ifndef MAP_REDUCE_MAPBASE_H
#define MAP_REDUCE_MAPBASE_H

#include "ValueType.h"
#include <bits/unique_ptr.h>

class MapBase {
public:
    virtual std::unique_ptr<ValueType> map(const std::unique_ptr<ValueType> &value) = 0;
};


#endif //MAP_REDUCE_MAPBASE_H
