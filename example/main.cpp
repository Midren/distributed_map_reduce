// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <vector>
#include <string>

#include "../map_reduce.h"

int main() {
//    std::vector<std::string> map_ips = {"127.0.0.1"};
//    const std::string reduce_address = "127.0.0.1:8001";
//    const std::string master_address = "127.0.0.1:8002";
    std::vector<std::string> map_ips = {"172.17.0.2", "172.17.0.3", "172.17.0.4", "172.17.0.5"};
    const std::string reduce_address = "172.17.0.6:8001";
    const std::string master_address = "172.17.0.7:8002";

    fs::path home_dir("/home/map_reduce");
    fs::path map_input_file = home_dir / "distributed_map_reduce/example/input.csv";
    fs::path dll_path = home_dir / "distributed_map_reduce/example/build/libmap_reduce_config.so";

    auto[key, value] = run_task_blocking(map_ips, reduce_address, master_address, map_input_file, dll_path);

    std::cout << "The result of Map/Reduce is " << value->to_string() << std::endl;

    return 0;
}
