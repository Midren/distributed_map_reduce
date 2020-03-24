#include <string>

#include "../configurator/JobConfig.h"
#include "../configurator/config.h"
#include "../util.h"

void groupby(const std::string &json) {
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);
    auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_out_factory);
    std::cout << key->to_string() << " " << value->to_string() << std::endl;
}

