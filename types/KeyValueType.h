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
};

//TODO: add SFINAE to instantiate only with pod types
template<typename T>
class PrimitiveKeyValueType : public KeyValueType {
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
