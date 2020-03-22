#include <iostream>
#include <fstream>
#include <filesystem>

#include "util.h"
#include "types/KeyValueType.h"
#include "configurator/JobConfig.h"
#include "configurator/util.h"


std::string read_data(const std::filesystem::path &) {
    std::ifstream fin{"input.csv"};
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open input file");
    }
    return dynamic_cast<std::stringstream &>(std::stringstream{} << fin.rdbuf()).str();
}

int main() {
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);

    auto data = read_data("input.csv");

    auto key_value_ins = get_key_values_from_csv(data, cfg->key_in_factory, cfg->value_in_factory);

    std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> key_value_outs;

    std::transform(key_value_ins.begin(), key_value_ins.end(), std::back_inserter(key_value_outs),
                   [&cfg](const auto &key_value) {
                       const auto &[key, value] = key_value;
                       return cfg->map_class->map(key, value);
                   });

    std::cout << to_csv(key_value_outs);
    //TODO: Add sending to reduce node
}