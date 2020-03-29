// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <filesystem>

#include <boost/asio.hpp>
#include <boost/asio/use_future.hpp>

#include "util.h"
#include "configurator/config.h"
#include "ssh/Node.h"

namespace fs = std::filesystem;

std::future<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>
get_result(const std::shared_ptr<JobConfig> &cfg) {
    std::promise<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> promise;
    auto future = promise.get_future();

    std::thread([](std::promise<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>> promise,
                   const std::shared_ptr<JobConfig> &cfg) {
        using namespace boost::asio::ip;
        boost::asio::io_context io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8002));
        tcp::socket socket(io_service);
        acceptor.async_accept(socket, [&socket, cfg, &promise](const boost::system::error_code &err) {
            std::array<char, 128> buf{};
            boost::system::error_code error;
            std::string json;
            for (;;) {
                size_t len = socket.read_some(boost::asio::buffer(buf), error);
                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw boost::system::system_error(error); // Some other error.
                json += std::string(buf.data(), len);
            }
            if (cfg->key_out_factory == nullptr ||
                cfg->value_res_factory == nullptr) {
                throw std::runtime_error("Key/value factories are null");
            }
            auto[key, value] = get_key_value_from_json(json, cfg->key_out_factory, cfg->value_res_factory);
            promise.set_value(std::make_pair(std::move(key), std::move(value)));
        });
        io_service.run();
    }, std::move(promise), std::cref(cfg)).detach();

    return future;
}

std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>
get_result_blocking(const std::shared_ptr<JobConfig> &cfg) {
    auto future = get_result(cfg);
    future.wait();
    return future.get();
}

void prepare_node(Node &n) {
    n.connect();
    n.execute_command("mkdir ~/.cache/mapreduce", false);
}

int main() {
    auto[master_ip, master_port] = parse_ip_port("127.0.0.1:8002");
    auto[reduce_ip, reduce_port] = parse_ip_port("127.0.0.1:8001");
    auto map_ip = "127.0.0.1";
    fs::path dll_path("/home/midren/ucu/distributed_db/DistributedMapReduce/cmake-build-debug/libmap_reduce_config.so");

    auto library_handler = get_config_dll_handler(dll_path.filename());
    auto cfg = get_config(library_handler);

    auto future = get_result(cfg);

    try {
        ssh_init();
        std::cout << map_ip << " " << reduce_ip << std::endl;
        Node map_node(map_ip), reduce_node(reduce_ip);
        reduce_node.connect();
        reduce_node.scp_send_file(dll_path, fs::path("/home/midren/.cache/mapreduce/libreduce_config.so"));
        reduce_node.execute_command("chmod +x ~/.cache/mapreduce/libreduce_config.so", false);
        reduce_node.execute_command(
                "cd ~/.cache/mapreduce && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/midren/.cache/mapreduce/ &&"
                "nohup /home/midren/ucu/distributed_db/DistributedMapReduce/cmake-build-debug/reduce_node --input_num 4 --master_node_address 127.0.0.1:8002 --port 8001 --config_file libreduce_config.so > reduce.out 2> reduce.err < /dev/null &",
                false);
        map_node.connect();
        map_node.scp_send_file(
                fs::path(
                        "/home/midren/ucu/distributed_db/DistributedMapReduce/cmake-build-debug/libmap_reduce_config.so"),
                fs::path("/home/midren/.cache/mapreduce/libmap_config.so"));
        map_node.execute_command("chmod +x ~/.cache/mapreduce/libmap_config.so", false);
        map_node.execute_command(
                "cd ~/.cache/mapreduce && export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/midren/.cache/mapreduce/ &&"
                "/home/midren/ucu/distributed_db/DistributedMapReduce/cmake-build-debug/map_node --input_file input.csv --reduce_node_address 127.0.0.1:8001 --config_file libmap_config.so > map.out 2> map.err < /dev/null",
                false);
        ssh_finalize();
    } catch (ssh::SshException &e) {
        std::cerr << e.getError() << std::endl;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    future.wait();
    auto[key, value] = future.get();

    std::cout << "The result of Map/Reduce is " << value->to_string() << std::endl;

    return 0;
}
