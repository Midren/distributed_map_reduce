// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <numeric>
#include "JobConfig.h"

class SquareMap : public MapBase {
public:
    std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
    map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) override {
        const auto input_value = dynamic_cast<IntKeyValueType *>(value.get());
        return {std::make_unique<IntKeyValueType>(dynamic_cast<IntKeyValueType *>(key.get())->value),
                std::make_unique<IntKeyValueType>(input_value->value * input_value->value)};
    };
};

class SumReduce : public ReduceBase {
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

JobConfig *get_config() {
    return &job_config;
}

JobConfig job_config = JobConfig("sum of squares",
                                 std::move(std::make_unique<SquareMap>()),
                                 std::move(std::make_unique<SumReduce>()),
                                 std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                 std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                 std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                 std::move(std::make_unique<IntKeyValueTypeFactory>()),
                                 std::move(std::make_unique<IntKeyValueTypeFactory>())
);
