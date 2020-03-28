// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <fstream>
#include <filesystem>

#include "../util.h"
#include "../configurator/JobConfig.h"
#include "../configurator/config.h"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>


namespace po = boost::program_options;

po::variables_map parse_args(int argc, char **argv) {
    po::options_description allowed("Allowed options");
    allowed.add_options()
            ("help,h", "Show help")
            ("config_file,c", po::value<std::filesystem::path>()->default_value("libmap_reduce_config.so"),
             "Path to config dll for map/reduce")
            ("input_file,I", po::value<std::filesystem::path>()->required(), "Path to input file with data to map")
            ("reduce_node_address,O", po::value<std::string>()->required(), "Address of reduce node");

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

std::string read_data(const std::filesystem::path &) {
    std::ifstream fin{"input.csv"};
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open input file");
    }
    return dynamic_cast<std::stringstream &>(std::stringstream{} << fin.rdbuf()).str();
}

std::pair<std::string, int> parse_ip_port(const std::string &address) {
    std::vector<std::string> ip_port;
    boost::split(ip_port, address, boost::is_any_of(":"));
    if (ip_port.size() != 2) {
        throw std::runtime_error("Address should have format ip:port");
    }
    return {ip_port[0], std::stoi(ip_port[1])};
}

int main(int argc, char **argv) {
    auto vm = parse_args(argc, argv);
    auto input_file = vm["input_file"].as<std::filesystem::path>();
    auto dll_file = vm["config_file"].as<std::filesystem::path>();
    auto[ip_address, port] = parse_ip_port(vm["reduce_node_address"].as<std::string>());

    auto library_handler = get_config_dll_handler(dll_file);
    auto cfg = get_config(library_handler);

    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip_address), port);
    boost::asio::io_service service;

    auto key_value_ins = get_key_values_from_csv(read_data(input_file), cfg->key_in_factory, cfg->value_in_factory);
    std::for_each(key_value_ins.begin(), key_value_ins.end(),
                  [&cfg, &ep, &service](const auto &key_value) {
                      const auto &[key, value] = key_value;;
                      auto res = cfg->map_class->map(key, value);

                      boost::asio::ip::tcp::socket sock(service);
                      try {
                          sock.connect(ep);
                          sock.write_some(boost::asio::buffer(to_json(res)));
                          sock.close();
                      } catch (std::exception &e) {
                          std::cerr << "Couldn't send data to reduce node: " << e.what() << std::endl;
                          exit(1);
                      }
                  });
}
