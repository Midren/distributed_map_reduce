#ifndef MAP_REDUCE_REDUCE_H
#define MAP_REDUCE_REDUCE_H

#include <iostream>
#include <string>

#include "../configurator/JobConfig.h"
#include "../configurator/config.h"
#include "../util.h"

template<typename T>
struct ptr_less {
    bool operator()(const T &lhs, const T &rhs) const {
        return *lhs.get() < *(rhs).get();
    }
};

std::map<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>, ptr_less<std::unique_ptr<KeyValueType>>> key_values;

constexpr int map_cnt = 4;

void process(const std::string &json) {
    auto library_handler = get_config_dll_handler("libmap_reduce_config.so");
    auto cfg = get_config(library_handler);
    auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_out_factory);
    auto it = key_values.find(key);
    if (it == key_values.end()) {
        if (map_cnt == 1) {
            auto values = std::vector<std::unique_ptr<KeyValueType>>{};
            values.push_back(std::move(value));
            //TODO: push values to thread-safe queue
        } else {
            key_values[std::move(key)].push_back(std::move(value));
        }
    } else {
        if (it->second.size() == map_cnt - 1) {
            auto values = std::move(it->second);
            values.push_back(std::move(value));
            key_values.erase(it);
            for (auto &item : values)
                std::cout << item->to_string() << std::endl;
            //TODO: push values to thread-safe queue
        } else {
            key_values[std::move(key)].push_back(std::move(value));
        }
    }
//    values[key].push_back(std::move(value));
}

#endif //MAP_REDUCE_REDUCE_H
