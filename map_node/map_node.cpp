// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>

#include "../util.h"
#include "../configurator/job_config.h"
#include "../configurator/config.h"

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

using namespace map_reduce;
namespace po = boost::program_options;

po::variables_map parse_args(int argc, char **argv) {
    po::options_description allowed("Allowed options");
    allowed.add_options()
            ("help,h", "Show help")
            ("config_file,c", po::value<std::filesystem::path>()->default_value("libmap_reduce_config.so"),
             "Path to job_config dll for map/reduce")
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

std::string read_data(const std::filesystem::path &file) {
    std::ifstream fin{file};
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open input file");
    }
    return dynamic_cast<std::stringstream &>(std::stringstream{} << fin.rdbuf()).str();
}

void
process_part(std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>::iterator beg,
             std::vector<std::pair<std::unique_ptr<KeyValueType>, std::unique_ptr<KeyValueType>>>::iterator end,
             const std::shared_ptr<job_config> &cfg,
             const boost::asio::ip::tcp::endpoint &reduce_ep) {
    boost::asio::io_service service;
    std::for_each(beg, end,
                  [&cfg, &reduce_ep, &service](const auto &key_value_input) {
                      const auto &[key_in, value_in] = key_value_input;;
                      //TODO: add working queue, to which will map_class emit resutlting key/value pairs and send
                      // data during processing others 
                      auto key_value_pairs = cfg->map_class->clone()->run_map(key_in, value_in);

                      for (const auto &key_value_res: key_value_pairs) {
                          boost::system::error_code ec;
                          boost::asio::ip::tcp::socket sock(service);
                          sock.connect(reduce_ep, ec);
                          sock.non_blocking(false);
                          if (ec)
                              throw std::runtime_error("cannot connect to reduce node");
                          sock.wait(sock.wait_write);
                          boost::asio::write(sock, boost::asio::buffer(to_json(key_value_res)),
                                             boost::asio::transfer_all(),
                                             ec);
                          if (ec)
                              throw std::runtime_error("fail during writing to socket");
                      }
                  });
}

int main(int argc, char **argv) {
    auto vm = parse_args(argc, argv);
    auto input_file = vm["input_file"].as<std::filesystem::path>();
    auto dll_file = vm["config_file"].as<std::filesystem::path>();
    auto[ip_address, port] = parse_ip_port(vm["reduce_node_address"].as<std::string>());

    auto library_handler = get_config_dll_handler(dll_file);
    auto cfg = get_config(library_handler);

    boost::asio::ip::tcp::endpoint reduce_ep(boost::asio::ip::address::from_string(ip_address), port);

    auto key_value_ins = get_key_values_from_csv(read_data(input_file), cfg->key_in_factory, cfg->value_in_factory);

    process_part(key_value_ins.begin(), key_value_ins.end(), cfg, reduce_ep);
//    std::vector<std::thread> thread_vector;
//    constexpr int THREAD_NUM = 4;
//    thread_vector.reserve(THREAD_NUM);
//    double step = key_value_ins.size() / static_cast<double>(THREAD_NUM);
//    for (unsigned int i = 0; i < THREAD_NUM - 1; i++) {
//        thread_vector.emplace_back(process_part, key_value_ins.begin() + std::floor(i * step),
//                                   key_value_ins.begin() + std::floor((i + 1) * step),
//                                   std::cref(cfg),
//                                   std::cref(reduce_ep));
//    }
//    process_part(key_value_ins.begin() + std::floor((THREAD_NUM - 1) * step), key_value_ins.end(), cfg, reduce_ep);
}
