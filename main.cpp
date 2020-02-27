#include <iostream>
#include <numeric>
#include <algorithm>
#include <functional>

#include "dlfcn.h"
#include "JobConf.h"

int main() {
    auto library_handler = dlopen("libmap_reduce_configurator.so", RTLD_LAZY);

    if (library_handler == nullptr) {
        std::cout << "Cannot open shared library: " << dlerror() << std::endl;
    }

    auto get_cfg = (get_config_t) dlsym(library_handler, "get_config");
    auto cfg = get_cfg();

    std::vector<int> real_inputs(4);
    std::iota(real_inputs.begin(), real_inputs.end(), 0);
    std::vector<std::unique_ptr<KeyValueType>> keys, inputs;
    std::transform(real_inputs.begin(), real_inputs.end(), std::back_inserter(keys), [](const auto &x) {
        return std::make_unique<IntKeyValueType>(0);
    });
    std::transform(real_inputs.begin(), real_inputs.end(), std::back_inserter(inputs), [](const auto &x) {
        return std::make_unique<IntKeyValueType>(x);
    });

    std::vector<std::unique_ptr<KeyValueType>> outputs;
    std::transform(keys.begin(), keys.end(), inputs.begin(), std::back_inserter(outputs),
                   [&](const auto &key, const auto &value) {
                       return cfg->getMapClass()->map(key, value).second;
                   });

    std::cout << dynamic_cast<IntKeyValueType *>(cfg->getReduceClass()->reduce(keys[0], outputs).second.get())->value;
    dlclose(library_handler);

    return 0;
}
