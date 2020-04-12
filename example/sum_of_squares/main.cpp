// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <vector>
#include <string>

#include "../../map_reduce.h"
#include "../../types/KeyValueType.h"

namespace fs = std::filesystem;

int main() {
    std::vector<std::string> map_ips = {"172.17.0.2", "172.17.0.3", "172.17.0.4", "172.17.0.5"};
    const std::string reduce_address = "172.17.0.6:8001";
    const std::string master_address = "172.17.0.7:8002";

    fs::path home_dir(getenv("HOME"));
    std::vector<fs::path> map_input_files;
    for (size_t i = 0; i < map_ips.size(); i++) {
        map_input_files.emplace_back(home_dir / "distributed_map_reduce/example/sum_of_squares/input.csv");
    }
    fs::path dll_path = home_dir / "distributed_map_reduce/example/sum_of_squares/build/libmap_reduce_config.so";

    auto res = map_reduce::run_task_blocking(map_ips, reduce_address, master_address, map_input_files, dll_path);
    std::cout << "The result of Map/Reduce is:" << std::endl;
    while (!res.empty()) {
        auto[key, value] = std::move(res.back());
        std::cout << "(" << key->to_string() << ", " << value->to_string() << ")" << std::endl;
        res.pop_back();
    }

    return 0;
}
