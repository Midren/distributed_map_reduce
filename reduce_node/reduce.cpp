// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "reduce.h"

#include <iostream>

#include <boost/asio.hpp>

#include "../util.h"

namespace map_reduce {

//TODO: add hash method for KeyValueType and replace to TBD concurrent_hashmap instead of map + mutex
    static std::map<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>, ComparePointee> key_values;
    static std::mutex map_mutex;


    void
    reduce(const std::shared_ptr<ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>>> &q,
           const std::shared_ptr<job_config> &cfg, const boost::asio::ip::tcp::endpoint &ep) {
        for (;;) {
            auto[key, values] = q->pop();
            auto res = cfg->reduce_class->reduce(key, values);

            boost::asio::io_service service;
            boost::asio::ip::tcp::socket sock(service);
            try {
                sock.connect(ep);
                sock.write_some(boost::asio::buffer(to_json(res)));
                sock.close();
            } catch (std::exception &e) {
                throw std::runtime_error("Couldn't send data to master node: " + std::string(e.what()));
            }
        }
    }

    void
    process(const std::shared_ptr<ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>> >>> &queue,
            const std::string &json, int map_cnt, const std::shared_ptr<job_config> &cfg) {
        std::cout << "One more input" << std::endl;
        try {
            auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_out_factory);
            map_mutex.lock();
            auto it = key_values.find(key);
            if (it == key_values.end()) {
                key_values[std::move(key)].push_back(std::move(value));
                map_mutex.unlock();
            } else {
                if (it->second.size() == map_cnt - 1) {
                    auto values = std::move(it->second);
                    values.push_back(std::move(value));
                    key_values.erase(it);
                    map_mutex.unlock();
                    std::cout << "All data for this key has come" << std::endl;
                    queue->push(make_pair(std::move(key), std::move(values)));
                } else {
                    key_values[std::move(key)].push_back(std::move(value));
                    map_mutex.unlock();
                    std::cout << "Added one more input" << std::endl;
                }
            }
        } catch (map_ended &e) {
            for (auto it = key_values.begin(); it != key_values.end(); it++) {
                auto nh = key_values.extract(it);
                queue->push(std::make_pair(std::move(nh.key()), std::move(nh.mapped())));
            }
        }
    }
}
