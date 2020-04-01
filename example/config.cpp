// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <numeric>
#include <memory>
#include "../configurator/job_config.h"

using map_reduce::KeyValueType;
using map_reduce::IntKeyValueType;
using map_reduce::IntKeyValueTypeFactory;

class square_map : public map_reduce::map_base {
public:
    std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) override {
        const auto input_value = dynamic_cast<IntKeyValueType *>(value.get());
        return {std::make_unique<IntKeyValueType>(dynamic_cast<IntKeyValueType *>(key.get())->value),
                std::make_unique<IntKeyValueType>(input_value->value * input_value->value)};
    };
};

class sum_reduce : public map_reduce::reduce_base {
public:
    std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    reduce(const std::unique_ptr<KeyValueType> &key,
           const std::vector<std::unique_ptr<KeyValueType>> &outputs) override {
        return {std::make_unique<IntKeyValueType>(dynamic_cast<IntKeyValueType *>(key.get())->value),
                std::make_unique<IntKeyValueType>(
                        std::accumulate(outputs.begin(), outputs.end(), 0, [](int lhs, const auto &rhs) {
                            return lhs + dynamic_cast<IntKeyValueType *>(rhs.get())->value;
                        }))};
    }
};

std::shared_ptr<job_config> get_config() {
    return std::make_shared<job_config>("sum of squares",
                                       std::move(std::make_unique<square_map>()),
                                       std::move(std::make_unique<sum_reduce>()),
                                       std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                       std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                       std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                       std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                       std::move(std::make_unique<IntKeyValueTypeFactory>())
    );
}
