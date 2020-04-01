// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_KEYVALUETYPE_H
#define MAP_REDUCE_KEYVALUETYPE_H

#include <string>
#include <sstream>

class KeyValueType {
public:
    virtual void parse(const std::string &) = 0;

    virtual std::string to_string() const = 0;

    bool operator<(const KeyValueType &lhs) {
        return this->to_string() < lhs.to_string();
    }

    virtual ~KeyValueType() = default;
};

template<typename T, class Enable =void>
class PrimitiveKeyValueType;

template<typename T>
class PrimitiveKeyValueType<T, typename std::enable_if_t<std::is_pod_v<T>>> : public KeyValueType {
public:
    PrimitiveKeyValueType() = default;

    explicit PrimitiveKeyValueType(T i) : value(i) {}

    void parse(const std::string &str) override {
        std::stringstream(str) >> value;
    }

    std::string to_string() const override {
        return std::to_string(value);
    }

    T value;
};

using CharKeyValueType = PrimitiveKeyValueType<char>;
using IntKeyValueType  = PrimitiveKeyValueType<int>;
using DoubleKeyValueType = PrimitiveKeyValueType<double>;
using LongKeyValueType = PrimitiveKeyValueType<long>;
#endif //MAP_REDUCE_KEYVALUETYPE_H
