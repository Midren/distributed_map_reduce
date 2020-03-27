// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_KEYVALUETYPEFACTORY_H
#define MAP_REDUCE_KEYVALUETYPEFACTORY_H

#include <bits/unique_ptr.h>
#include "KeyValueType.h"

class KeyValueTypeFactory {
public:
    virtual std::unique_ptr<KeyValueType> create() = 0;

    virtual std::unique_ptr<KeyValueType> create(const std::string &str) = 0;
};

template<typename T>
class PrimitiveKeyValueTypeFactory : public KeyValueTypeFactory {
public:
    std::unique_ptr<KeyValueType> create() override {
        return std::make_unique<PrimitiveKeyValueType<T>>();
    };

    std::unique_ptr<KeyValueType> create(const std::string &str) override {
        std::unique_ptr<KeyValueType> ptr = this->create();
        ptr->parse(str);
        return ptr;
    };
};

using CharKeyValueTypeFactory = PrimitiveKeyValueTypeFactory<char>;
using IntKeyValueTypeFactory  = PrimitiveKeyValueTypeFactory<int>;
using DoubleKeyValueTypeFactory = PrimitiveKeyValueTypeFactory<double>;
using LongKeyValueTypeFactory = PrimitiveKeyValueTypeFactory<long>;
#endif //MAP_REDUCE_KEYVALUETYPEFACTORY_H
