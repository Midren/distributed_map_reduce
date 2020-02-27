#include <numeric>
#include "JobConf.h"

class SquareMap : public MapBase {
public:
    std::unique_ptr<ValueType> map(const std::unique_ptr<ValueType> &value) override {
        const auto input_value = dynamic_cast<IntValueType *>(value.get());
        return std::make_unique<IntValueType>(input_value->value * input_value->value);
    };
};

class SumReduce : public ReduceBase {
public:
    std::unique_ptr<ValueType> reduce(const std::vector<std::unique_ptr<ValueType>> &outputs) override {
        return std::make_unique<IntValueType>(
                std::accumulate(outputs.begin(), outputs.end(), 0, [](int &lhs, const auto &rhs) {
                    return lhs += dynamic_cast<IntValueType *>(rhs.get())->value;
                }));
    }
};

JobConf *get_config() {
    return &job_config;
}

JobConf job_config = JobConf("sum of squares", std::make_unique<SquareMap>(), std::make_unique<SumReduce>());
