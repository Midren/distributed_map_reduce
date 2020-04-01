// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef MAP_REDUCE_MAP_REDUCE_H
#define MAP_REDUCE_MAP_REDUCE_H

#include <filesystem>
#include <vector>
#include <memory>
#include <string>
#include <future>

#include "types/KeyValueType.h"

namespace fs = std::filesystem;

std::future<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
run_task(const std::vector<std::string> &map_ips, const std::string &reduce_address, const std::string &master_address,
         const fs::path &map_input_file, const fs::path &dll_path);

std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
run_task_blocking(const std::vector<std::string> &map_ips, const std::string &reduce_address,
                  const std::string &master_address, const fs::path &map_input_file, const fs::path &dll_path);

#endif //MAP_REDUCE_MAP_REDUCE_H
