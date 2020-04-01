// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <filesystem>

#include <boost/program_options.hpp>

#include "json_server.h"
#include "reduce.h"

#include "../configurator/config.h"
#include "../util.h"

namespace po = boost::program_options;

po::variables_map parse_args(int argc, char **argv) {
    po::options_description allowed("Allowed options");
    allowed.add_options()
            ("help,h", "Show help")
            ("config_file,c", po::value<std::filesystem::path>()->default_value("libmap_reduce_config.so"),
             "Path to JobConfig dll for map/reduce")
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
    auto master_ip_port = parse_ip_port(vm["master_node_address"].as<std::string>());
    auto map_cnt = vm["input_num"].as<unsigned int>();
    auto port = vm["port"].as<unsigned int>();

    auto library_handler = get_config_dll_handler(dll_file);
    auto cfg = get_config(library_handler);

    boost::asio::io_context io_service;

    JsonServer s(io_service, port, [&](const std::string &json) {
        return process(json, map_cnt, cfg);
    });

    io_service.run();
}

