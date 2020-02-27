#ifndef MAP_REDUCE_VALUETYPE_H
#define MAP_REDUCE_VALUETYPE_H

#include <string>

class ValueType {
public:
    virtual void is_implemented() = delete;
};

template<typename T>
class PrimitiveValueType : public ValueType {
public:
    explicit PrimitiveValueType(T i) : value(i) {}

    T value;
};

using CharValueType = PrimitiveValueType<char>;
using IntValueType = PrimitiveValueType<int>;
using DoubleValueType = PrimitiveValueType<double>;
using LongValueType = PrimitiveValueType<long>;
#endif //MAP_REDUCE_VALUETYPE_H
