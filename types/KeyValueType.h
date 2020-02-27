#ifndef MAP_REDUCE_KEYVALUETYPE_H
#define MAP_REDUCE_KEYVALUETYPE_H

#include <string>

class KeyValueType {
public:
    virtual void is_implemented() = delete;
};

template<typename T>
class PrimitiveKeyValueType : public KeyValueType {
public:
    explicit PrimitiveKeyValueType(T i) : value(i) {}

    T value;
};

using CharKeyValueType = PrimitiveKeyValueType<char>;
using IntKeyValueType  = PrimitiveKeyValueType<int>;
using DoubleKeyValueType = PrimitiveKeyValueType<double>;
using LongKeyValueType = PrimitiveKeyValueType<long>;
#endif //MAP_REDUCE_KEYVALUETYPE_H
