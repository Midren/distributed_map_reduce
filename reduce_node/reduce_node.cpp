// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <filesystem>

#include <boost/program_options.hpp>

#include "concurrent_queue.h"
#include "json_server.h"
#include "reduce.h"

#include "../configurator/config.h"
#include "../util.h"

using namespace map_reduce;
namespace po = boost::program_options;

po::variables_map parse_args(int argc, char **argv) {
    po::options_description allowed("Allowed options");
    allowed.add_options()
            ("help,h", "Show help")
            ("config_file,c", po::value<std::filesystem::path>()->default_value("libmap_reduce_config.so"),
             "Path to job_config dll for map/reduce")
            ("port,p", po::value<unsigned int>()->required(), "Port, on which server will be running")
            ("master_node_address,O", po::value<std::string>()->required(), "Address of master node")
            ("input_num,n", po::value<unsigned int>()->required(), "Number of inputs per key");

    po::options_description cmdline_options;
    cmdline_options.add(allowed);

    po::variables_map vm;
    po::positional_options_description p;
    po::store(po::command_line_parser(argc, argv).
            options(cmdline_options).positional(p).run(), vm);

    if (vm.count("help")) {
        allowed.print(std::cout);
        exit(0);
    }

    po::notify(vm);

    return vm;
}

int main(int argc, char **argv) {
    auto vm = parse_args(argc, argv);
    auto dll_file = vm["config_file"].as<std::filesystem::path>();
    auto[master_ip_str, master_port] = parse_ip_port(vm["master_node_address"].as<std::string>());
    auto master_ep = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(master_ip_str),
                                                    master_port);
    auto map_cnt = vm["input_num"].as<unsigned int>();
    auto port = vm["port"].as<unsigned int>();

    auto library_handler = get_config_dll_handler(dll_file);
    auto cfg = get_config(library_handler);

    boost::asio::io_context io_service;
    auto queue = std::make_shared<ConcurrentQueue<std::pair<std::unique_ptr<KeyValueType>, std::vector<std::unique_ptr<KeyValueType>>>>>();

    auto json_handler = std::make_shared<std::function<void(const std::string &)>>(
            [&](const std::string &json) mutable {
                return process(queue, json, map_cnt, cfg);
            });

    json_server s(io_service, port, json_handler);

    std::vector<std::thread> server_thread_vector, reduce_thread_vector;
    constexpr int THREAD_NUM = 4;
    server_thread_vector.reserve(THREAD_NUM);
    reduce_thread_vector.reserve(THREAD_NUM);
    for (auto i = 0; i < THREAD_NUM; i++)
        server_thread_vector.emplace_back([&io_service] { io_service.run(); });

    for (auto i = 0; i < THREAD_NUM; i++)
        reduce_thread_vector.emplace_back(reduce, std::ref(queue), std::cref(cfg), std::cref(master_ep));

    for (auto &thread: reduce_thread_vector)
        thread.join();

    io_service.stop();
}
