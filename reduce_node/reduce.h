// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_REDUCE_H
#define MAP_REDUCE_REDUCE_H

#include <string>
#include <memory>
#include <map>
#include <vector>

#include <boost/asio/ip/tcp.hpp>

#include "../types/KeyValueType.h"
#include "../configurator/job_config.h"

#include "concurrent_queue.h"

namespace map_reduce {
    void
    reduce(const std::shared_ptr<ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>>> &q,
           const std::shared_ptr<job_config> &cfg, const boost::asio::ip::tcp::endpoint &ep);

    void
    process(const std::shared_ptr<ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>>> &q,
            const std::string &json, int map_cnt, const std::shared_ptr<job_config> &cfg);

}
#endif //MAP_REDUCE_REDUCE_H
