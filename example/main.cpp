// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <vector>
#include <string>

#include "../map_reduce.h"

int main() {
    std::vector<std::string> map_ips = {"172.17.0.2", "127.0.0.3", "127.0.0.4", "127.0.0.5"};
    const std::string reduce_address = "172.17.0.6:8001";
    const std::string master_address = "127.0.0.1:8002";

    fs::path map_input_file = "~/distributed_map_reduce/example/input.csv";
    fs::path dll_path("/home/midren/ucu/distributed_db/DistributedMapReduce/cmake-build-debug/libmap_reduce_config.so");

    auto[key, value] = run_task_blocking(map_ips, reduce_address, master_address, map_input_file, dll_path);

    std::cout << "The result of Map/Reduce is " << value->to_string() << std::endl;

    return 0;
}
