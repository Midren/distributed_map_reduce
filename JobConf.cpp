#include <numeric>
#include "JobConf.h"

class SquareMap : public MapBase {
public:
    std::unique_ptr<KeyValueType> map(const std::unique_ptr<KeyValueType> &key, const std::unique_ptr<KeyValueType> &value) override {
        const auto input_value = dynamic_cast<IntKeyValueType *>(value.get());
        return std::make_unique<IntKeyValueType>(input_value->value * input_value->value);
    };
};

class SumReduce : public ReduceBase {
public:
    std::unique_ptr<KeyValueType> reduce(const std::unique_ptr<KeyValueType> &key, const std::vector<std::unique_ptr<KeyValueType>> &outputs) override {
        return std::make_unique<IntKeyValueType>(
                std::accumulate(outputs.begin(), outputs.end(), 0, [](int &lhs, const auto &rhs) {
                    return lhs += dynamic_cast<IntKeyValueType *>(rhs.get())->value;
                }));
    }
};

JobConf *get_config() {
    return &job_config;
}

JobConf job_config = JobConf("sum of squares", std::make_unique<SquareMap>(), std::make_unique<SumReduce>());
