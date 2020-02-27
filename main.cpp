#include <iostream>
#include <numeric>
#include <algorithm>
#include <functional>

#include "dlfcn.h"
#include "JobConf.h"

int main() {
    auto *library_handler = dlopen("libmap_reduce_configurator_library.so", RTLD_LAZY);
    if (library_handler == nullptr) {
        std::cout << "Cannot open shared library: " << dlerror() << std::endl;
    }
    auto get_cfg = (get_config_t) dlsym(library_handler, "get_config");
    auto cfg = get_cfg();
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
