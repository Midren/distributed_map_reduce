#ifndef MAP_REDUCE_MAPBASE_H
#define MAP_REDUCE_MAPBASE_H

#include "types/KeyValueType.h"
#include <bits/unique_ptr.h>

class MapBase {
public:
    virtual std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) = 0;
};


#endif //MAP_REDUCE_MAPBASE_H
