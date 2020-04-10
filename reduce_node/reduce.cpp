// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "reduce.h"

#include <iostream>

#include <boost/asio.hpp>
#include <queue>

#include "../util.h"

namespace map_reduce {
    struct ComparePointee {
        template<typename T>
        bool operator()(const std::unique_ptr<T> &lhs, const std::unique_ptr<T> &rhs) const {
            return *lhs < *rhs;
        }
    };

    struct CompareFirstPointee {
        template<typename T>
        bool operator()(const std::pair<std::unique_ptr<T>, std::vector<std::unique_ptr<T>>> &lhs,
                        const std::pair<std::unique_ptr<T>, std::vector<std::unique_ptr<T>>> &rhs) const {
            return ComparePointee()(lhs.first, rhs.first);
        }
    };

    //TODO: add hash method for KeyValueType and replace to TBD concurrent_hashmap instead of map + mutex
    static std::map<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>, ComparePointee> key_values;
    static unsigned int map_finished;
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
    process(const std::shared_ptr<ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>>> &working_queue,
            const std::string &json, int map_cnt, const std::shared_ptr<job_config> &cfg) {
        std::cout << "One more input" << std::endl;
        try {
            auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_out_factory);
            std::lock_guard lg(map_mutex);
            key_values[std::move(key)].push_back(std::move(value));
        } catch (map_ended &e) {
            std::cout << "Map node sent all data" << std::endl;
            std::lock_guard lg(map_mutex);
            map_finished++;

            if (map_finished == map_cnt) {
                std::cout << "All inputs were inserted" << std::endl;
                std::priority_queue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>,
                        std::vector<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>>,
                        CompareFirstPointee
                > high_priority_queue;

                while (!key_values.empty()) {
                    auto nh = key_values.extract(key_values.begin());
                    high_priority_queue.push(std::make_pair(std::move(nh.key()), std::move(nh.mapped())));
                }
                while (!high_priority_queue.empty()) {
                    working_queue->push(std::move(
                            const_cast<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>> &>(high_priority_queue.top())));
                    high_priority_queue.pop();
                }
            }
        }
    }
}
