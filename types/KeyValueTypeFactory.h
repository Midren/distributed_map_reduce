// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_KEYVALUETYPEFACTORY_H
#define MAP_REDUCE_KEYVALUETYPEFACTORY_H

#include <bits/unique_ptr.h>
#include "KeyValueType.h"

namespace map_reduce {
    class KeyValueTypeFactory {
    public:
        virtual std::unique_ptr<KeyValueType> create() = 0;

        virtual std::unique_ptr<KeyValueType> create(const std::string &str) {
            std::unique_ptr<KeyValueType> ptr = this->create();
            ptr->parse(str);
            return ptr;
        }
    };

    template<typename T, typename = std::enable_if_t<std::is_pod_v<T>>>
    class PrimitiveKeyValueTypeFactory : public KeyValueTypeFactory {
    public:
        std::unique_ptr<KeyValueType> create() override {
            return std::make_unique<PrimitiveKeyValueType<T>>();
        };
    };

    using CharKeyValueTypeFactory = PrimitiveKeyValueTypeFactory<char>;
    using IntKeyValueTypeFactory  = PrimitiveKeyValueTypeFactory<int>;
    using DoubleKeyValueTypeFactory = PrimitiveKeyValueTypeFactory<double>;
    using LongKeyValueTypeFactory = PrimitiveKeyValueTypeFactory<long>;

    class StringKeyValueTypeFactory : public KeyValueTypeFactory {
    public:
        std::unique_ptr<KeyValueType> create() override {
            return std::make_unique<StringKeyValueType>();
        };
    };
}
#endif //MAP_REDUCE_KEYVALUETYPEFACTORY_H
