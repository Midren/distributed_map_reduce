// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_REDUCE_H
#define MAP_REDUCE_REDUCE_H

#include <iostream>
#include <string>

#include "concurrent_queue.h"
#include "../configurator/JobConfig.h"
#include "../configurator/config.h"
#include "../util.h"

struct ComparePointee {
    template<typename T>
    bool operator()(const std::unique_ptr<T> &lhs, const std::unique_ptr<T> &rhs) const {
        return *lhs < *rhs;
    }
};

//TODO: add hash method for KeyValueType and replace to TBD concurrent_hashmap instead of map + mutex
std::map<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>, ComparePointee> key_values;
std::mutex map_mutex;

ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>> queue;

void
reduce(ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>> &q,
       const std::shared_ptr<JobConfig> &cfg) {
    auto[key, values] = q.pop();
    auto res = cfg->reduce_class->reduce(key, values);

    boost::asio::io_service service;
    //TODO: remove hardcoded constants
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("172.17.0.7"), 8002);
    boost::asio::ip::tcp::socket sock(service);
    try {
        sock.connect(ep);
        sock.write_some(boost::asio::buffer(to_json(res)));
        sock.close();
    } catch (std::exception &e) {
        throw std::runtime_error("Couldn't send data to master node: " + std::string(e.what()));
    }
}

void
process(const std::string &json, int map_cnt, const std::shared_ptr<JobConfig> &cfg) {
    auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_out_factory);
    auto it = key_values.find(key);
    if (it == key_values.end()) {
        if (map_cnt == 1) {
            auto values = std::vector<std::unique_ptr<KeyValueType>>{};
            values.push_back(std::move(value));
            queue.push(make_pair(std::move(key), std::move(values)));
            //TODO: make actually concurrent
            reduce(queue, cfg);
        } else {
            key_values[std::move(key)].push_back(std::move(value));
        }
    } else {
        if (it->second.size() == map_cnt - 1) {
            auto values = std::move(it->second);
            values.push_back(std::move(value));
            key_values.erase(it);
            queue.push(make_pair(std::move(key), std::move(values)));
            //TODO: make actually concurrent
            reduce(queue, cfg);
        } else {
            key_values[std::move(key)].push_back(std::move(value));
        }
    }
}

#endif //MAP_REDUCE_REDUCE_H
