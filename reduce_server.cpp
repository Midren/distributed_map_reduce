#include <iostream>

#include "util.h"
#include "types/KeyValueType.h"
#include "configurator/JobConfig.h"
#include "configurator/util.h"


int main() {
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);

    //TODO: Add server to get connections from map nodes and get data
    std::string data;

    auto key_value_outs = get_key_values_from_csv(data, cfg->key_in_factory, cfg->value_in_factory);

    //TODO: Group by key_out

    //TODO: Run reduce on data

    //TODO: Send result somewhere
    std::cout << to_csv(key_value_outs);
}
