#include <iostream>
#include <numeric>
#include <algorithm>
#include <functional>
#include "MapBase.h"
#include "ReduceBase.h"

int main() {
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

    std::vector<int> real_inputs(4);
    std::iota(real_inputs.begin(), real_inputs.end(), 0);

    std::vector<std::unique_ptr<ValueType>> inputs;
    std::transform(real_inputs.begin(), real_inputs.end(), std::back_inserter(inputs), [](const auto &x) {
        return std::make_unique<IntValueType>(x);
    });

    std::vector<std::unique_ptr<ValueType>> outputs;
    std::transform(inputs.begin(), inputs.end(), std::back_inserter(outputs),
                   std::bind(&MapBase::map, cfg->getMapClass(), std::placeholders::_1));

    std::cout << dynamic_cast<IntValueType *>(cfg->getReduceClass()->reduce(outputs).get())->value;

    return 0;
}
